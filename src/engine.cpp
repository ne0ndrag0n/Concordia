#include "bbtypes.hpp"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "utility.hpp"
#include "lotentity.hpp"
#include "lot.hpp"
#include "engine.hpp"
#include "json/json.h"
#include <iterator>
#include <string>
#include <vector>
#include <iostream>

namespace BlueBear {

	Engine::Engine() {
		L = luaL_newstate();
		luaL_openlibs( L );
	}

	Engine::~Engine() {
		lua_close( L );
	}

	/**
	 * Setup the global environment all Engine mods will run within. This method sets up required global objects used by each mod.
	 */
	bool Engine::setupRootEnvironment() {
		// TODO: The root script should not be a user-modifiable file, but rather a hardcoded minified string. This script sets up
		// the root Lua environment all mods (including pack-ins) run from, and is NOT to be changed by the user.
		if ( luaL_dofile( L, LUASPHERE_MAIN ) ) {
			printf( "Failed to set up BlueBear root environment: %s\n", lua_tostring( L, -1 ) );
			return false;
		}

		// Setup the root environment by loading in and "class-ifying" all objects used by the game
		Utility::doDirectories( L, BLUEBEAR_MODPACK_DIRECTORY );

		return true;
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
			if( parseSuccessful ) {
				// Log some basic information about the loading of the lot
				std::cout << "[" << lotPath << "] " << "Lot revision: " << lotJSON[ "rev" ] << std::endl;

				// Instantiate the lot
				currentLot.reset(
					new BlueBear::Lot(
						lotJSON[ "floorx" ].asInt(),
						lotJSON[ "floory" ].asInt(),
						lotJSON[ "stories" ].asInt(),
						lotJSON[ "subtr" ].asInt(),
						BlueBear::TerrainType( lotJSON[ "terrain" ].asInt() )
					)
				);

				// Create one lot table for the Luasphere - contains functions that we call on this->currentLot to do things like get other objects on the lot and trigger events
				createLotTable();

				// Set world ticks to the one saved in the file
				worldTicks = lotJSON[ "ticks" ].asInt();

				// Clear the std::map containing all objects
				currentLot->objects.clear();

				// Iterate through the "entities" array
				Json::Value entities = lotJSON[ "entities" ];
				Json::FastWriter writer;
				for( Json::Value& entity : entities ) {
					std::string classID = entity[ "classID" ].asString();
					// Dump JSON to string (LotEntity requires strings in its constructor)
					std::string instance = writer.write( entity[ "instance" ] );
					std::string cid = entity[ "instance" ][ "_cid" ].asString();

					// Emplace the object into the std::map (insert the new object as we create it)
					currentLot->objects.emplace( cid, BlueBear::LotEntity( L, classID, instance ) );
				}

				// After the lot has all its LotEntities loaded, let's fix those serialized function references
				deserializeFunctionRefs();
			}
		} else {
			std::cout << "Unable to load lot: " << lotPath << std::endl;
			return false;
		}

		return true;
	}

	/**
	 * For all serialized curried functions in _sys._sched, ask each object to deserialize the reference to another LotEntity
	 */
	void Engine::deserializeFunctionRefs() {

		// Get fresh Lua stack
		Utility::clearLuaStack( L );

		for( auto& keyValuePair : currentLot->objects ) {
			BlueBear::LotEntity& currentEntity = keyValuePair.second;

			// Push this object's table onto the API stack
			lua_rawgeti( L, LUA_REGISTRYINDEX, currentEntity.luaVMInstance );
			// Get the reserved function "_deserialize_function_refs"
			Utility::getTableValue( L, "_deserialize_function_refs" );
			// First argument is that same table (the context)
			lua_pushvalue( L, -2 );

			// Call _deserialize_function_refs: The rest is done in Lua
			if( lua_pcall( L, 1, 0, 0 ) != 0 ) {
				std::cout << lua_tostring( L, -1 ) << std::endl;
				currentEntity.ok = false;
			}
		}
	}

	/**
	 * We do this once; create the lot table and assign it a lot instance
	 */
	void Engine::createLotTable() {

		// Get "dumb pointer" from smart pointer
		Lot* lot = currentLot.get();

		// Push the "bluebear" global onto the stack, then push the "lot" identifier
		// We will set this at the very end of the function
		lua_getglobal( L, "bluebear" );
		lua_pushstring( L, "lot" );

		// Push new, blank table
		lua_createtable( L, 0, 1 );

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

		// Remember pushing the bluebear table, then lot? Stack should now have the lot table,
		// the "lot" identifier, then the bluebear global. Go ahead and set "lot" to this table.
		lua_settable( L, -3 );
	}

	/**
	 * Where the magic happens
	 */
	void Engine::objectLoop() {
		std::cout << "Starting world engine with a tick count of " << worldTicks << "\n";

		// Push the bluebear global onto the stack - leave it there
		lua_getglobal( L, "bluebear" );
		// Push table value "current_tick" onto the stack - leave it there too
		Utility::getTableValue( L, "engine" );

		for( ; worldTicks != WORLD_TICKS_MAX; worldTicks++ ) {
			// Set current_tick on bluebear.lot (inside the Luasphere, system/root.lua) to the current tick
			Utility::setTableIntValue( L, "current_tick", worldTicks );

			for( auto& keyValuePair : currentLot->objects ) {
				BlueBear::LotEntity& currentEntity = keyValuePair.second;

				// Execute object if it is "ok"
				if( currentEntity.ok == true ) {
					// currentEntity.execute should leave the stack as it was when it was called!!
					currentEntity.execute();
				}
			}
		}

		std::cout << "Finished!" << std::endl;
	}

	/**
	 * Given the cid of a player, the cid of an object, and its desired action, call the action method on the LotEntity
	 * by passing the player identified by playerId as the "player" argument.
	 */
	void Engine::callActionOnObject( const char* playerId, const char* obejctId, const char* method ) {

	}

}
