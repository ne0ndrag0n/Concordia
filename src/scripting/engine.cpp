#include "bbtypes.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "tools/utility.hpp"
#include "scripting/lot.hpp"
#include "scripting/engine.hpp"
#include "graphics/display.hpp"
#include "configmanager.hpp"
#include "eventmanager.hpp"
#include "scripting/infrastructurefactory.hpp"
#include "scripting/luakit/serializer.hpp"
#include "log.hpp"
#include <jsoncpp/json/json.h>
#include <iterator>
#include <string>
#include <sstream>
#include <vector>
#include <chrono>
#include <ratio>
#include <thread>
#include <memory>
#include <list>
#include <stdexcept>

namespace BlueBear {
	namespace Scripting {

		Engine::Engine( EventManager& eventManager ) :
		 eventManager( eventManager ),
		 lastExecuted( std::chrono::steady_clock::now() ),
		 L( luaL_newstate() ),
		 ticksPerSecond( 1000 / ConfigManager::getInstance().getIntValue( "fps_overview" ) ),
		 currentModpackDirectory( nullptr ),
		 cancel( false ) {
			luaL_openlibs( L );
			setActiveState( false );

			setupEvents();
		}

		Engine::~Engine() {
			lua_close( L );
		}

		/**
		 * Hook into eventmanager for everything we need
		 */
		void Engine::setupEvents() {
			eventManager.UI_ACTION_EVENT.listen( this, [ & ]( LuaReference function ) {
				waitingTable.waitForTick( currentTick + 1, function );
			} );
		}

		/**
		 * Setup the global environment all Engine mods will run within. This method sets up required global objects used by each mod.
		 */
		bool Engine::submitLuaContributions() {

			// bluebear
			lua_newtable( L );

			// bluebear.engine
			lua_pushstring( L, "engine" );
			lua_newtable( L );

			// bluebear.engine.get_all_objects retrieves all objects
			lua_pushstring( L, "get_all_objects" );
			lua_pushlightuserdata( L, this );
			lua_pushcclosure( L, &Engine::lua_getLotObjects, 1 );
			lua_settable( L, -3 );

			// bluebear.engine.get_objects_by_type gets all objects on the lot of a specific type
			lua_pushstring( L, "get_objects_by_type" );
			lua_pushlightuserdata( L, this );
			lua_pushcclosure( L, &Engine::lua_getLotObjectsByType, 1 );
			lua_settable( L, -3 );

			// bluebear.engine.require_modpack
			lua_pushstring( L, "require_modpack" );
			lua_pushlightuserdata( L, this );
			lua_pushcclosure( L, &Engine::lua_loadModpack, 1 );
			lua_settable( L, -3 );

			// bluebear.engine.setup_stemcell
			lua_pushstring( L, "setup_stemcell" );
			lua_pushlightuserdata( L, this );
			lua_pushcclosure( L, &Engine::lua_setupStemcell, 1 );
			lua_settable( L, -3 );

			// bluebear.engine.set_timeout
			lua_pushstring( L, "set_timeout" );
			lua_pushlightuserdata( L, this );
			lua_pushcclosure( L, &Engine::lua_setTimeout, 1 );
			lua_settable( L, -3 );

			// bluebear.engine.tick_rate
			lua_pushstring( L, "tick_rate" );
			lua_pushnumber( L, ticksPerSecond );
			lua_settable( L, -3 );

			// Set the engine table on "bluebear"
			lua_settable( L, -3 );

			// bluebear.config
			lua_pushstring( L, "config" );
			lua_newtable( L );

			// bluebear.config.get_value
			lua_pushstring( L, "get_value" );
			lua_pushlightuserdata( L, &( ConfigManager::getInstance() ) );
			lua_pushcclosure( L, &ConfigManager::lua_getValue, 1 );
			lua_settable( L, -3 );

			// Set the config table on "bluebear"
			lua_settable( L, -3 );

			// bluebear.util
			lua_pushstring( L, "util" );
			lua_newtable( L );

			// bluebear.util.get_directory_list
			lua_pushstring( L, "get_directory_list" );
			lua_pushcfunction( L, &Tools::Utility::lua_getFileList );
			lua_settable( L, -3 );

			// bluebear.util.get_pointer
			lua_pushstring( L, "get_pointer" );
			lua_pushcfunction( L, &Tools::Utility::lua_getPointer );
			lua_settable( L, -3 );

			// Set the util table on "bluebear"
			lua_settable( L, -3 );

			// Set the table as "bluebear"
			lua_setglobal( L, "bluebear" );

			// Override the default print function
			lua_register( L, "print", &Engine::lua_print );

			// Integrate system modpacks
			if( !loadModpackSet( SYSTEM_MODPACK_DIRECTORY ) ) {
				return false;
			}
			// System modpacks should not remain part of the modpack list
			loadedModpacks.clear();
			// Integrate standard objects
			if( !loadModpackSet( BLUEBEAR_MODPACK_DIRECTORY ) ) {
				return false;
			}

			// Set up an InfrastructureFactory to load things like floor tiles and wallpapers
			infrastructureFactory = std::make_unique< InfrastructureFactory >();
			infrastructureFactory->registerFloorTiles();
			infrastructureFactory->registerWallpapers();

			return true;
		}

		/**
		 * Load a set of modpacks given a parent directory (const char* as they are ROM constants)
		 */
		bool Engine::loadModpackSet( const char* modpackDirectory ) {
			currentModpackDirectory = modpackDirectory;

			auto modpacks = Tools::Utility::getSubdirectoryList( modpackDirectory );

			for( auto& modpack : modpacks ) {
				if( !loadModpack( modpack ) ) {
					return false;
				}
			}

			return true;
		}

		/**
		 * Load a Modpack. The mod name will be prepended with BLUEBEAR_MODPACK_DIRECTORY.
		 * @param		{std::string}		name
		 * @returns	{bool}		True if the modpack was or is integrated successfully: false otherwise.
		 */
		bool Engine::loadModpack( const std::string& name ) {
			// If this modpack is LOADING, don't load it twice! This is a circular dependency; a modpack being imported by another modpack called
			// to load the first modpack (which was still LOADING)! Fail immediately.
			// Fail immediately if it already failed (don't waste time loading it again)
			if( loadedModpacks[ name ] == ModpackStatus::LOADING || loadedModpacks[ name ] == ModpackStatus::LOAD_FAILED ) {
				return false;
			}

			// Don't load another modpack twice. Another script might have already loaded this modpack. Just go "uh-huh" and
			// remind the source call that this modpack was already loaded.
			if( loadedModpacks[ name ] == ModpackStatus::LOAD_SUCCESSFUL ) {
				return true;
			}

			// Assemble the fully-qualified pathname for this modpack
			std::string path = currentModpackDirectory + name;
			std::string fullPath = path + "/" + MODPACK_MAIN_SCRIPT;

			// Mark the module as LOADING - first if should catch this module if it's called again without completing
			loadedModpacks[ name ] = ModpackStatus::LOADING;

			// dofile pointed to by path
			if( luaL_loadfile( L, fullPath.c_str() ) || !lua_pushstring( L, path.c_str() ) || lua_pcall( L, 1, LUA_MULTRET, 0 ) ) {
				// Exception occurred during opening the modpack
				// Exception occurred during the integration of this modpack
				Log::getInstance().error( "Engine::loadModpack", "Failed to integrate modpack " + name + ": " + lua_tostring( L, -1 ) );
				lua_pop( L, 1 );
				loadedModpacks[ name ] = ModpackStatus::LOAD_FAILED;
				return false;
			}

			loadedModpacks[ name ] = ModpackStatus::LOAD_SUCCESSFUL;
			return true;
		}

		/**
		 * Lua C++ binding to Engine::loadModpack
		 */
		int Engine::lua_loadModpack( lua_State* L ) {
			// Because Lua requires methods be static in C closure, pop the first argument: the "this" pointer
			Engine* engine = ( Engine* )lua_touserdata( L, lua_upvalueindex( 1 ) );

			// Get the modpack name
			std::string modpack( lua_tostring( L, -1 ) );
			lua_pop( L, 1 );

			// Modpack name can't be blank
			if( modpack == "" ) {
				return luaL_error( L, "Modpack name not specified!" );
			}

			// If loadModpack fails, throw an exception and fail this entire attempt at loading a modpack
			// loadModpack will return false if the dependency is circular, or the dependency is not found.
			// loadModpack will return true if the dependency was loaded successfully one time.
			if( !engine->loadModpack( modpack ) ) {
				return luaL_error( L, "Failed to load modpack: %s", modpack.c_str() );
			}

			return 0;
		}

		/**
		 * Lua C++ binding to create a stemcell
		 */
		 int Engine::lua_setupStemcell( lua_State* L ) {
			 	Engine* engine = ( Engine* )lua_touserdata( L, lua_upvalueindex( 1 ) );

				// Expected stack: stemcell_type stemcell
				if( lua_isstring( L, -1 ) ) {
					std::string stemcellType( lua_tostring( L, -1 ) );
					lua_pop( L, 1 );

					return luaL_error( L, "'%s' is not a valid stemcell type.", stemcellType.c_str() );
				}

				return 0;
		 }

		/**
		 * Load a lot
		 */
		bool Engine::loadLot( const char* lotPath ) {
			// Get an ifstream from the given lot
			std::ifstream lot( lotPath );

			// Verify it is both open and good
			if( lot.is_open() && lot.good() ) {

				// Set up JsonCpp options
				// lotJSON is the root JSON object for the lot
				Json::Value fileJSON;
				// reader is an object used to parse the std::ifstream file into a JSON object
				Json::Reader reader;
				bool parseSuccessful = reader.parse( lot, fileJSON );

				// If parse was successful, begin loading the lot
				if( parseSuccessful && fileJSON.isObject() ) {
					Json::Value lotJSON = fileJSON[ "lot" ];
					Json::Value engineJSON = fileJSON[ "engine" ];

					// Log some basic information about the loading of the lot
					Log::getInstance().info( "Engine::loadLot", "[" + std::string( lotPath ) + "] Lot revision: " + fileJSON[ "rev" ].asString() );

					// Set world ticks to the one saved in the file
					currentTick = engineJSON[ "ticks" ].asInt();

					// Instantiate the lot
					currentLot = std::make_shared< Lot >( L, *infrastructureFactory, lotJSON );

					// Clear the std::map containing all objects
					objects.clear();

					// Deserialize the world
					LuaKit::Serializer serializer( L );
					objects = serializer.loadWorld( engineJSON, *this );
				} else {
					Log::getInstance().error( "Engine::loadLot", "Unable to parse " + std::string( lotPath ) );
					return false;
				}
			} else {
				Log::getInstance().error( "Engine::loadLot", "Unable to parse " + std::string( lotPath ) );
				return false;
			}

			return true;
		}

		/**
		 * Sets the active state of the loop. Typically done from an EngineCommand.
		 */
		void Engine::setActiveState( bool status ) {
			active = status;

			if( active == true ) {
				// Cleared for takeoff
				sleepInterval = 1000;
			} else {
				sleepInterval = 300;
			}
		}

		/**
		 * Where the magic happens
		 */
		void Engine::objectLoop() {

			// Move items waiting for this tick out of the waiting table into the callback queue
			waitingTable.triggerTick( currentTick );

			// If there are any callbacks, update bluebear.engine.current_tick
			if( !waitingTable.queuedCallbacks.empty() ) {
				lua_getglobal( L, "bluebear" ); // bluebear
				lua_pushstring( L, "engine" ); // "engine" bluebear
				lua_gettable( L, -2 ); // bluebear.engine bluebear

				lua_pushstring( L, "current_tick" ); // "current_tick" bluebear.engine bluebear
				lua_pushnumber( L, currentTick ); // currentTick "current_tick" bluebear.engine bluebear
				lua_settable( L, -3 ); // bluebear.engine bluebear

				lua_pop( L, 2 ); // EMPTY

				// Burn out every function scheduled for this tick
				while( !waitingTable.queuedCallbacks.empty() ) {
					LuaReference function = waitingTable.queuedCallbacks.front();
					lua_rawgeti( L, LUA_REGISTRYINDEX, function ); // <function>

					if( int stat = lua_pcall( L, 0, 0, 0 ) ) { // error
						Log::getInstance().error( "Engine::objectLoop", "Exception thrown on tick " + std::to_string( currentTick ) + ": " + ( stat == -1 ? "<C++ exception>" : lua_tostring( L, -1 ) ) );
						lua_pop( L, 1 ); // EMPTY
					} // EMPTY

					// Only YOU can prevent memory leaks!
					// The "function" reference should have not been used anywhere else in the pipeline (enqueued to now)
					luaL_unref( L, LUA_REGISTRYINDEX, function );

					waitingTable.queuedCallbacks.pop();
				}
			}

			// On every tick, increment currentTick
			currentTick++;

		}

		/**
		 * Overrides the default Lua print() function to go through the logger
		 */
		 int Engine::lua_print( lua_State* L ) {

			 try {
				 int numArgs = lua_gettop( L );

				 // print( "message" )
				 if( numArgs == 1 ) {
					 Log::getInstance().debug( "LuaScript", lua_tostring( L, -1 ) );
				 }

				 // print( "tag", "message" )
				 else if( numArgs == 2 ) {
					 Log::getInstance().debug( lua_tostring( L, -2 ), lua_tostring( L, -1 ) );
				 }
			 } catch ( std::logic_error e ) {
				 Log::getInstance().error( "lua_print", "Failed to print() log message (did you use tostring() on non-string objects?)" );
			 }

			 return 0;
		 }

		 /**
		  *
			* STACK ARGS: interval function
			* RETURNS: stringhandle
			*/
		 int Engine::lua_setTimeout( lua_State* L ) {

			 Engine* self = ( Engine* )lua_touserdata( L, lua_upvalueindex( 1 ) );

			 // First argument on the stack should be a function
			 // The rightmost argument is on the top of the stack
			 if( lua_isfunction( L, -2 ) && lua_isnumber( L, -1 ) ) {
				 // warning: watch the type of Tick
				 Tick interval = lua_tointeger( L, -1 );

				 lua_pop( L, 1 ); // function

				 // This reference should be a reference owned by the queue-waiting table pair, and should never be used anywhere else.
				 // When the engine consumes the function, the reference should be luaL_unref'd and freed.
				 // If you don't do this, you'll get horriffic memory leaks that slowly worsen as a lot is played.
				 LuaReference function = luaL_ref( L, LUA_REGISTRYINDEX ); // EMPTY

				 if( interval == 0 ) {
					 // This function was scheduled to run in the same tick, but when the current stack has completed.
					 // You GENERALLY shouldn't do this but it's got its use cases.
					 self->waitingTable.queuedCallbacks.push( function );

					 // You'll get a nil handle because a callback scheduled to go on the current tick cannot be cancelled.
					 lua_pushnil( L ); // nil
				 } else {
					 // This function was scheduled to run at tick+1 or after
					 std::string handle = self->waitingTable.waitForTick( self->currentTick + interval, function );

					 lua_pushstring( L, handle.c_str() ); // "handle"
				 }

				 return 1;
			 } else {
				 Log::getInstance().warn( "Engine::lua_setTimeout", "Invalid arguments provided to bluebear.engine.set_timeout: Argument 1 is a function and argument 2 is a number." );

				 return 0;
			 }
		 }

		 int Engine::lua_getLotObjects( lua_State* L ) {

			 // Pop the lot off the stack
			 Engine* engine = ( Engine* )lua_touserdata( L, lua_upvalueindex( 1 ) );

			 // Create an array table with as many entries as the size of this->objects
			 lua_createtable( L, engine->objects.size(), 0 );

			 // Push 'em on!
			 size_t tableIndex = 1;
			 for( int lotEntity : engine->objects ) {
				 lua_rawgeti( L, LUA_REGISTRYINDEX, lotEntity );
				 lua_rawseti( L, -2, tableIndex++ );
			 }

			 return 1;

		 }

		 int Engine::lua_getLotObjectsByType( lua_State* L ) {

			 // Because Lua requires methods be static in C closure, pop the first argument: the "this" pointer
			 Engine* engine = ( Engine* )lua_touserdata( L, lua_upvalueindex( 1 ) );

			 // Get argument (the class we are looking for) and remove it from the stack
			 // Copy using modern C++ string methods into archaic, unsafe C-string format used by Lua API
			 std::string keystring( lua_tostring( L, -1 ) );
			 const char* idKey = keystring.c_str();
			 lua_pop( L, 1 );

			 // This table will be the array of matching lot
			 lua_newtable( L );

			 // Start at index number 1 - Lua arrays (tables) start at 1
			 size_t tableIndex = 1;

			 // Iterate through each object on the lot, checking to see if each is an instance of "idKey"
			 for( int lotEntity : engine->objects ) {
				 // Push bluebear global
				 lua_getglobal( L, "bluebear" );

				 // Push instance_of utility function
				 Tools::Utility::getTableValue( L, "instance_of" );

				 // Push the two arguments: identifier, and instance
				 lua_pushstring( L, idKey );
				 lua_rawgeti( L, LUA_REGISTRYINDEX, lotEntity );

				 // We're ready to call instance_of on object!
				 if( lua_pcall( L, 2, 1, 0 ) == 0 ) {
					 // Read the answer left on the stack as a boolean
					 // lua_getboolean actually returns an int, which we'll need interpreted as a bool
					 bool isInstance = !!lua_toboolean( L, -1 );

					 // Before we do anything else, get rid of the result, the instance_of function
					 // and the bluebear tableIndex from the stack - we don't need them anymore
					 // until the loop restarts, and this will put our return table back at the top
					 // of the stack.
					 lua_pop( L, 2 );

					 // If this object is a descendant of idKey, push it onto the table
					 if( isInstance ) {
						 // Re-push the instance onto the stack
						 lua_rawgeti( L, LUA_REGISTRYINDEX, lotEntity );
						 // Push it onto the table on our stack
						 lua_rawseti( L, -2, tableIndex++ );
					 }
				 }
			 }

			 return 1;

		 }
	}
}
