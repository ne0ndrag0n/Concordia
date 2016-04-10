#include "bbtypes.hpp"
#include "utility.hpp"
#include "lotentity.hpp"
#include "lot.hpp"
#include "engine.hpp"
#include "json.hpp"
#include <iterator>
#include <string>
#include <vector>
#include <iostream>

extern "C" {
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

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
		if ( luaL_dofile( L, "system/root.lua" ) ) {
			printf( "Failed to set up BlueBear root environment: %s\n", lua_tostring( L, -1 ) );
			return false;
		}

		// Setup the root environment by loading in and "class-ifying" all objects used by the game
		Utility::doDirectories( L, BLUEBEAR_OBJECTS_DIRECTORY );

		return true;
	}

	/**
	 * Load a lot
	 *
	 * TODO: wtf do we do with these goddamn type mismatches
	 */
	bool Engine::loadLot( const char* lotPath ) {
		int lotTableRef;
		std::ifstream lot( lotPath );

		if( lot.is_open() && lot.good() ) {

			// Shitty library using exceptions!!
			try {
				json lotJSON( lot );

				std::cout << "[" << lotPath << "] " << "Lot revision: " << lotJSON[ "rev" ] << std::endl;

				// Instantiate the lot
				int terrain = lotJSON[ "terrain" ];
				this->currentLot = new BlueBear::Lot(
					lotJSON[ "floorx" ],
					lotJSON[ "floory" ],
					lotJSON[ "stories" ],
					lotJSON[ "subtr" ],
					BlueBear::TerrainType( terrain )
				);

				// Create one lot table for the Luasphere - contains functions that we call on this->currentLot to do things like get other objects on the lot and trigger events
				lotTableRef = this->createLotTable( this->currentLot );

				this->worldTicks = lotJSON[ "ticks" ];

				this->currentLot->objects.clear();

				// Iterate through the "entities" array: each object within is a serialised LotEntity
				json entities = lotJSON[ "entities" ];
				for( json element : entities ) {
					std::string classID = element[ "classID" ];
					std::string instance = element[ "instance" ].dump();

					// Dump JSON to string for Luasphere to create into new object and extend over
					BlueBear::LotEntity obj( this->L, classID.c_str(), instance.c_str(), lotTableRef );

					// obj.ok will be true if we completed successfully
					if( obj.ok == true ) {
						this->currentLot->objects.push_back( obj );
					} else {
						// TODO: standard console log needed with cross-platform color abstraction required
						std::cout << "\033[1;33m" << "Warning: Failed to instantiate lot object " << element[ "classID" ] << "\033[0m" << std::endl;
					}
				}
			} catch( ... ) {
				std::cout <<  "Failed to load lot: Library threw exception for lot " << lotPath <<  std::endl;
				return false;
			}

			/*
			// Create BBLotEntitys
			for( size_t i = 0; i != oitSize; i++ ) {
				// Each POP also begins with an index of the item in the OIT
				uint16_t odtIndex = Utility::getuint16_t( &lot );

				// This is the size of the POP
				uint16_t popSize = Utility::getuint16_t( &lot );

				// Create POP
				char pop[ popSize ] = { 0 };
				lot.read( pop, static_cast< int >( popSize ) );

				// Add object to Engine objects vector
				// BlueBear::LotEntity instances are wrappers around the Lua instances of the object
				BlueBear::LotEntity obj( this->L, objectIDs[ odtIndex ].typeKey.c_str(), pop, popSize, objectIDs[ odtIndex ].lotEntityType );
				// Set a reference to the lot table on this object
				obj.lotTableRef = lotTableRef;
				this->currentLot->objects.push_back( obj );
			}
			*/
		}

		return false;
	}

	/**
	 * We do this once; create the lot table and assign it a lot instance
	 */
	int Engine::createLotTable( Lot* lot ) {
		// Push new, blank table
		lua_createtable( this->L, 0, 1 );

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

		// Save the reference to this table
		return luaL_ref( L, LUA_REGISTRYINDEX );
	}

	/**
	 * Where the magic happens
	 */
	void Engine::objectLoop() {
		std::cout << "Starting world engine with a tick count of " << this->worldTicks << "\n";

		// Iterate for an entire week of ticks
		for( ; this->worldTicks != 50000; this->worldTicks++ ) {
			for( std::vector< BlueBear::LotEntity >::iterator object = this->currentLot->objects.begin(); object != this->currentLot->objects.end(); object++ ) {
				object->execute( this->worldTicks );
			}
		}

		std::cout << std::endl;
	}

	/**
	 * Given the cid of a player, the cid of an object, and its desired action, call the action method on the LotEntity
	 * by passing the player identified by playerId as the "player" argument.
	 */
	void Engine::callActionOnObject( const char* playerId, const char* obejctId, const char* method ) {

	}

}
