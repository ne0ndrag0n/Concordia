#include "bbtypes.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "tools/utility.hpp"
#include "scripting/lotentity.hpp"
#include "scripting/lot.hpp"
#include "scripting/engine.hpp"
#include "threading/commandbus.hpp"
#include "graphics/display.hpp"
#include "configmanager.hpp"
#include "scripting/eventmanager.hpp"
#include "scripting/infrastructurefactory.hpp"
#include "log.hpp"
#include <jsoncpp/json/json.h>
#include <iterator>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <chrono>
#include <ratio>
#include <thread>
#include <regex>
#include <memory>
#include <mutex>
#include <list>
#include <stdexcept>

namespace BlueBear {
	namespace Scripting {

		Engine::Engine( Threading::CommandBus& commandBus ) :
		 L( luaL_newstate() ), currentModpackDirectory( nullptr ), ticksPerSecond( ConfigManager::getInstance().getIntValue( "ticks_per_second" ) ), commandBus( commandBus ) {
			luaL_openlibs( L );
			setActiveState( false );
		}

		Engine::~Engine() {
			lua_close( L );
		}

		/**
		 * Setup the global environment all Engine mods will run within. This method sets up required global objects used by each mod.
		 */
		bool Engine::setupRootEnvironment() {

			// bluebear
			lua_newtable( L );

			// bluebear.engine
			lua_pushstring( L, "engine" );
			lua_newtable( L );

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

			return true;
		}

		/**
		 * Expose pieces of lot and eventmanager to the Luasphere
		 */
		void Engine::setupLotEnvironment() {
			Lot* lot = currentLot.get();
			EventManager* globalEventManager = eventManager.get();

			// Push the "bluebear" global onto the stack, then push the "lot" identifier
			// We will set this at the very end of the function
			lua_getglobal( L, "bluebear" );
			lua_pushstring( L, "lot" );

			// Push new, blank table
			lua_createtable( L, 0, 7 );

			// get_all_objects retrieves all objects
			lua_pushstring( L, "get_all_objects" );
			lua_pushlightuserdata( L, lot );
			lua_pushcclosure( L, &Lot::lua_getLotObjects, 1 );
			lua_settable( L, -3 );

			// get_objects_by_type gets all objects on the lot of a specific type
			lua_pushstring( L, "get_objects_by_type" );
			lua_pushlightuserdata( L, lot );
			lua_pushcclosure( L, &Lot::lua_getLotObjectsByType, 1 );
			lua_settable( L, -3 );

			// get_object_by_cid retrieves a specific object by its cid
			lua_pushstring( L, "get_object_by_cid" );
			lua_pushlightuserdata( L, lot );
			lua_pushcclosure( L, &Lot::lua_getLotObjectByCid, 1 );
			lua_settable( L, -3 );

			// create_new_instance creates a new instance of an entity and registers it with the lot engine
			lua_pushstring( L, "create_new_instance" );
			lua_pushlightuserdata( L, lot );
			lua_pushcclosure( L, &Lot::lua_createLotEntity, 1 );
			lua_settable( L, -3 );

			// listen_for instructs the Lot to listen for a specific broadcast for a specific object
			lua_pushstring( L, "listen_for" );
			lua_pushlightuserdata( L, globalEventManager );
			lua_pushcclosure( L, &EventManager::lua_registerEvent, 1 );
			lua_settable( L, -3 );

			// stop_listening_for instructs the Lot that an object is no longer listening for this broadcast
			lua_pushstring( L, "stop_listening_for" );
			lua_pushlightuserdata( L, globalEventManager );
			lua_pushcclosure( L, &EventManager::lua_unregisterEvent, 1 );
			lua_settable( L, -3 );

			// broadcast instructs the Lot to wake up all objects listening for the message that is broadcasted
			lua_pushstring( L, "broadcast" );
			lua_pushlightuserdata( L, globalEventManager );
			lua_pushcclosure( L, &EventManager::lua_broadcastEvent, 1 );
			lua_settable( L, -3 );

			// Remember pushing the bluebear table, then lot? Stack should now have the lot table,
			// the "lot" identifier, then the bluebear global. Go ahead and set "lot" to this table.
			lua_settable( L, -3 );

			// Pop bluebear
			lua_pop( L, 1 );
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

					// This leaves a lot to be desired
					if( stemcellType == "event-manager" ) {
						// "_inst" stemcell
						lua_pushstring( L, "_inst" );

						// ud "_inst" stemcell
	 					EventManager** userdata = ( EventManager** )lua_newuserdata( L, sizeof( EventManager* ) );
						*userdata = new EventManager( L, engine->currentLot );

						// table ud "_inst" stemcell
						lua_newtable( L );

						// "_gc" table ud "_inst" stemcell
						lua_pushstring( L, "__gc" );
						// closure() "_gc" table ud "_inst" stemcell
						lua_pushcfunction( L, EventManager::lua_gc );
						// table ud "_inst" stemcell
						lua_settable( L, -3 );

						// ud "_inst" stemcell
						lua_setmetatable( L, -2 );

						// Set self._inst to the userdata
						// stemcell
						lua_settable( L, -3 );

						// Transform the stemcell to an EventManager
						lua_pushstring( L, "listen_for" );
						lua_pushlightuserdata( L, *userdata );
						lua_pushcclosure( L, &EventManager::lua_registerEvent, 1 );
						lua_settable( L, -3 );

						lua_pushstring( L, "stop_listening_for" );
						lua_pushlightuserdata( L, *userdata );
						lua_pushcclosure( L, &EventManager::lua_unregisterEvent, 1 );
						lua_settable( L, -3 );

						lua_pushstring( L, "broadcast" );
						lua_pushlightuserdata( L, *userdata );
						lua_pushcclosure( L, &EventManager::lua_broadcastEvent, 1 );
						lua_settable( L, -3 );

						lua_pushstring( L, "load" );
						lua_pushlightuserdata( L, *userdata );
						lua_pushcclosure( L, &EventManager::lua_load, 1 );
						lua_settable( L, -3 );

					} else {
						return luaL_error( L, "'%s' is not a valid stemcell type.", stemcellType.c_str() );
					}
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
				Json::Value lotJSON;
				// reader is an object used to parse the std::ifstream file into a JSON object
				Json::Reader reader;
				bool parseSuccessful = reader.parse( lot, lotJSON );

				// If parse was successful, begin loading the lot
				if( parseSuccessful && lotJSON.isObject() ) {
					// Log some basic information about the loading of the lot
					Log::getInstance().info( "Engine::loadLot", "[" + std::string( lotPath ) + "] Lot revision: " + lotJSON[ "rev" ].asString() );

					// Set world ticks to the one saved in the file
					currentTick = lotJSON[ "ticks" ].asInt();

					// Instantiate the lot
					currentLot = std::make_shared< Lot >( L, currentTick, *infrastructureFactory, lotJSON );

					// Setup global event manager
					eventManager = std::make_unique< EventManager >( L, currentLot );
					if( lotJSON.isMember( "global_events" ) ) {
						eventManager->load( lotJSON[ "global_events" ] );
					}

					// Expose the lot and eventmanager methods to luasphere
					setupLotEnvironment();

					// Clear the std::map containing all objects
					currentLot->objects.clear();

					// Iterate through the "entities" array
					for( Json::Value& entity : lotJSON[ "entities" ] ) {
						currentLot->createLotEntityFromJSON( entity );
					}

					// After the lot has all its LotEntities loaded, let's fix those serialized function references
					if( !deserializeFunctionRefs() ) {
						// If we're not able to deserialise a function ref, this lot is broken, and cannot be used
						Log::getInstance().error( "Engine::loadLot", "Unable to load lot " + std::string( lotPath ) + ": This lot contains a missing entity." );
						return false;
					}
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
		 * For all serialized curried functions in _sys._sched, ask each object to deserialize the reference to another LotEntity
		 */
		bool Engine::deserializeFunctionRefs() {

			// Set up the regex
			std::regex serialTableReference( "^t\\/bb\\d+$" );

			// Get fresh Lua stack
			Tools::Utility::clearLuaStack( L );

			for( auto& keyValuePair : currentLot->objects ) {
				// Dereference the pointer to the LotEntity
				LotEntity& currentEntity = *( keyValuePair.second );

				// Push this object's table onto the API stack
				lua_rawgeti( L, LUA_REGISTRYINDEX, currentEntity.luaVMInstance );

				// Start with a new adventure in the stack!
				// Get system table
				// _sys
				Tools::Utility::getTableValue( L, "_sys" );
				// Get schedule table inside
				// _sys._sched _sys
				Tools::Utility::getTableValue( L, "_sched" );

				// For each key-value pair in _sys._sched, grab the arrays
				// nil _sys._sched _sys
				lua_pushnil( L );
				while( lua_next( L, -2 ) != 0 ) {
						// -1: the actual array, -2: the tick these functions have to execute
						// [SFTs] "1337.0" _sys._sched _sys

						// Now that we have an array of Serialised Function Tables (SFTs)
						// start conducting what needs to be done on *those*
						// nil [SFTs] "1337.0" _sys._sched _sys
						lua_pushnil( L );
						while( lua_next( L, -2 ) != 0 ) {
								// -1: the SFT, -2: its position
								// {SFT} 1 [SFTs] "1337.0" _sys._sched _sys

								// Grab the "arguments" array from the SFT
								Tools::Utility::getTableValue( L, "arguments" );

								if( lua_istable( L, -1 ) ) {
									// [arguments] {SFT} 1 [SFTs] "1337.0" _sys._sched _sys

									// One more...go through THAT array
									// nil [arguments] {SFT} 1 [SFTs] "1337.0" _sys._sched _sys
									lua_pushnil( L );
									while( lua_next( L, -2 ) != 0 ) {
										// -1: the argument, -2: its position
										// argument 1 [arguments] {SFT} 1 [SFTs] "1337.0" _sys._sched _sys

										// If the argument is a string, check if it fits the pattern "^t/bb\d+$"
										// and if it does, replace the value in that array with a dereferenced table
										if( lua_isstring( L, -1 ) ) {
											std::string argument( lua_tostring( L, -1 ) );

											if( std::regex_match( argument, serialTableReference ) ) {
												// Ask lot for numeric index of desired cid
												std::string bbId = argument.substr( 2 );
												int reference = currentLot->getLotObjectByCid( bbId );
												if( reference != -1 ) {
													// Push that object onto the stack
													// object_table argument 1 [arguments] {SFT} 1 [SFTs] "1337.0" _sys._sched _sys
													lua_rawgeti( L, LUA_REGISTRYINDEX, reference );
													// Get our current index at -3 and replace the value in table (at -4)
													// argument 1 [arguments] {SFT} 1 [SFTs] "1337.0" _sys._sched _sys
													lua_rawseti( L, -4, lua_tointeger( L, -3 ) );
												} else {
													// Fault tolerance. Very bad scenario! Game cannot continue...fail
													// Pop everything we've been doing...
													Tools::Utility::clearLuaStack( L );
													return false;
												}
											}
										}

										// Get rid of the argument, leaving the index for the next iteration of this table
										// 1 [arguments] {SFT} 1 [SFTs] "1337.0" _sys._sched _sys
										lua_pop( L, 1 );
									}
								} else {
									// nil {SFT} 1 [SFTs] "1337.0" _sys._sched _sys
								}

								// Pop the arguments table and SFT; we're done and ready for the next SFT
								// 1 [SFTs] "1337.0" _sys._sched _sys
								lua_pop( L, 2 );
						}
						// [SFTs] "1337.0" _sys._sched _sys

						// Pop the array of SFTs, and leave the previous key so we can work on the next one
						// "1337.0" _sys._sched _sys
						lua_pop( L, 1 );
				}
				// _sys.sched _sys

				// Pop these two; we don't need 'em anymore
				// <empty stack>
				lua_pop( L, 2 );

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
			Log::getInstance().debug( "Engine::objectLoop", "Starting world engine with a tick count of " + std::to_string( currentTick ) );

			// Push the bluebear global onto the stack - leave it there
			lua_getglobal( L, "bluebear" );
			// Push table value "current_tick" onto the stack - leave it there too
			Tools::Utility::getTableValue( L, "engine" );

			// This single container holds our list of commands to send to the display
			Graphics::Display::CommandList displayCommandList;
			// This pointer holds the direction to our list of incoming engine commands
			Scripting::Engine::CommandList engineCommandList;

			// Send infrastructure. When display is finished loading, it will send back the activation signal, changing the sleepInterval to a full second
			// and unlocking the main loop to perform other operations.
			displayCommandList.push_back( std::make_unique< Graphics::Display::SendInfrastructureCommand >( *currentLot ) );

			// This outer loop is a preliminary feature, the engine shouldn't stop until it's instructed to
			// We'll need to account for integer overflow in both here and Lua
			while( currentTick <= WORLD_TICKS_MAX ) {
				// Let's set up a start and end duration
				auto startTime = std::chrono::steady_clock::now();

				// Attempt to consume items in the engineCommandList
				commandBus.attemptConsume( engineCommandList );
				for( auto& command : engineCommandList ) {
	        command->execute( *this );
	      }
	      engineCommandList.clear();

				if( active == true ) {
					// ** START THE SINGLE TICK LOOP **
					// Complete a tick set: currentTick up to the next time it is evenly divisible by ticksPerSecond
					int ticksRemaining = ticksPerSecond;
					while( ticksRemaining-- ) {
						// On every tick, increment currentTick
						currentTick++;

						// Set current_tick on bluebear.lot (inside the Luasphere, system/root.lua) to the current tick
						Tools::Utility::setTableIntValue( L, "current_tick", currentTick );

						for( auto& keyValuePair : currentLot->objects ) {
							LotEntity& currentEntity = *( keyValuePair.second );

							// Execute object if it is "ok"
							if( currentEntity.ok == true ) {
								// currentEntity.execute should leave the stack as it was when it was called!!
								currentEntity.execute();
							}
						}
					}
					// ** END THE SINGLE TICK LOOP **
				}

				// Try to empty out the list and send it to the commandbus
				if( displayCommandList.size() > 0 ) {
					commandBus.attemptProduce( displayCommandList );
				}

				// Wait the difference between one second, and however long it took for this shit to finish
				// Guarantees that one second will elapse every "ticksPerSecond" ticks
				std::this_thread::sleep_until( startTime + std::chrono::milliseconds( sleepInterval ) );
			}

			Log::getInstance().debug( "Engine::objectLoop", "Finished!" );
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

		// ---------- COMMANDS ----------
		Engine::RegisterInstance::RegisterInstance( unsigned int instanceId )
			: instanceId( instanceId ) {}

		void Engine::RegisterInstance::execute( Engine& instance ) {
			std::stringstream ss;
			ss << "Registered this new entity " << instanceId;
			Log::getInstance().info( "RegisterInstance", ss.str() );
		}

		Engine::SetLockState::SetLockState( bool status ) : status( status ) {}
		void Engine::SetLockState::execute( Engine& instance ) {
			instance.setActiveState( status );
		}
	}
}
