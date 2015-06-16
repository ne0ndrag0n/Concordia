#include "utility.hpp"
#include "object.hpp"
#include "bluebear.hpp"
#include "bbtypes.hpp"
#include <chrono>
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cstdint>

// Not X-Platform
#include <dirent.h>

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
		
		// Gets a list of all directories in the "assets/objects" folder. Each folder holds everything required for a BlueBear object.
		std::vector< std::string > directories = Utility::getSubdirectoryList( BLUEBEAR_OBJECTS_DIRECTORY );
		
		// For each of these subdirectories, do the obj.lua file within our lua scope
		for ( std::vector< std::string >::iterator directory = directories.begin(); directory != directories.end(); directory++ ) {
			std::string scriptPath = BLUEBEAR_OBJECTS_DIRECTORY + *directory + "/obj.lua";
			if( luaL_dofile( L, scriptPath.c_str() ) ) {
				printf( "Error in BlueBear object: %s\n", lua_tostring( L, -1 ) );
			}
		}
		
		return true;
	}
	
	/**
	 * Load a lot
	 * 
	 * TODO: wtf do we do with these goddamn type mismatches
	 */ 
	bool Engine::loadLot( const char* lotPath ) {
		int lotTableRef;
		std::ifstream lot( lotPath, std::ifstream::binary );
		
		if( lot.is_open() && lot.good() ) {
			BlueBear::BBLTLotHeader lotHeader;
			lot.read( reinterpret_cast< char* >( &lotHeader ), 10 );
			
			if( Utility::swap_uint32( lotHeader.magicID ) == BLUEBEAR_LOT_MAGIC_ID ) {
				
				// Instantiate the lot
				this->currentLot = new BlueBear::Lot(
					static_cast< int >( lotHeader.lotX ),
					static_cast< int >( lotHeader.lotY ),
					static_cast< int >( lotHeader.numStories ),
					static_cast< int >( lotHeader.undergroundStories ),
					static_cast< BlueBear::TerrainType >( lotHeader.terrainType )
				);

				
				// Create one lot table for the Luasphere - contains functions that we call on this->currentLot to do things like get other objects on the lot and trigger events
				lotTableRef = this->createLotTable( this->currentLot );
				
				// This doesn't seem to work properly as part of the struct. don't know why, so get it separately instead
				uint32_t lotTime = Utility::getuint32_t( &lot );
				this->worldTicks = static_cast< unsigned int >( lotTime );
				
				// Load length of Object Definition Table (ODT)
				uint32_t odtSize = Utility::getuint32_t( &lot );

				// Read in the ODT
				char odt[ odtSize ] = { 0 };
				lot.read( odt, static_cast< int >( odtSize ) );
				std::vector< std::string > objectIDs;
				
				// Create ODT table
				char* odtPtr = odt;
				size_t index = 0;
				while ( index < static_cast< size_t >( odtSize ) ) {
					objectIDs.push_back( std::string( odtPtr ) );
					index = index + objectIDs.back().size() + 1;
					odtPtr += objectIDs.back().size() + 1;
				}
				
				// Verify each object exists
				if( !( this->verifyODT( objectIDs ) ) ) {
					std::cout << "An object in this lot does not exist in the global ODT!" << std::endl;
					return false;
				}
				
				// Get size of the OIT
				uint32_t oitSize = Utility::getuint32_t( &lot );
				
				this->currentLot->objects.clear();

				// Create BBObjects
				for( size_t i = 0; i != oitSize; i++ ) {
					// Each POP begins with an index of the item in the OIT
					uint16_t odtIndex = Utility::getuint16_t( &lot );
					
					// This is the size of the POP
					uint16_t popSize = Utility::getuint16_t( &lot );
					
					// Create POP
					char pop[ popSize ] = { 0 };
					lot.read( pop, static_cast< int >( popSize ) );
					
					// Add object to Engine objects vector
					// BlueBear::Object instances are wrappers around the Lua instances of the object
					BlueBear::Object obj( this->L, objectIDs.at( odtIndex ).c_str(), pop, popSize );
					// Set a reference to the lot table on this object
					obj.lotTableRef = lotTableRef;
					this->currentLot->objects.push_back( obj );
				}

				return true;
			} else {
				std::cout << "This doesn't appear to be a valid BlueBear lot.\n";
			}
		}
		
		std::cerr << "Couldn't load lot!\n";
		return false;
	}
	
	/**
	 * @private
	 * Verify that the Object Definition Table entries have corresponding objects in the Luasphere
	 * 
	 * @param		{std::vector< std::string >}	odt		The object definition table
	 */
	bool Engine::verifyODT( std::vector< std::string > odt ) {
		Utility::clearLuaStack( this->L );
		
		// Push _bbobjects onto Lua API stack
		lua_getglobal( this->L, "_bbobjects" );

		for ( std::vector< std::string >::iterator odtEntry = odt.begin(); odtEntry != odt.end(); odtEntry++ ) {
			Utility::getTableValue( this->L, odtEntry->c_str() );
			if( !lua_istable( this->L, -1 ) ) {
				std::cout << "Not a table!" << std::endl;
				lua_pop( L, 1 );
				return false;
			}
			lua_pop( L, 1 );
		}
		
		return true;
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
			for( std::vector< BlueBear::Object >::iterator object = this->currentLot->objects.begin(); object != this->currentLot->objects.end(); object++ ) {	
				object->execute( this->worldTicks );
			}
		}
		
		std::cout << std::endl;
	}
	
	Lot::Lot() {
		
	}
	
	Lot::Lot( int floorX, int floorY, int stories, int undergroundStories, BlueBear::TerrainType terrainType ) {
		this->floorX = floorX;
		this->floorY = floorY;
		
		this->stories = stories;
		this->undergroundStories = undergroundStories;
		
		this->terrainType = terrainType;
	}
	
	int Lot::lua_getLotObjects( lua_State* L ) {
		
		// Pop the lot off the stack
		BlueBear::Lot* lot = ( BlueBear::Lot* )lua_touserdata( L, lua_upvalueindex( 1 ) );
		
		size_t objectsLength = lot->objects.size();

		// Create an array table with as many entries as the size of this->objects
		lua_createtable( L, objectsLength, 0 );
		
		// Push 'em on!
		for( size_t index = 0; index != objectsLength; index++ ) {
			lua_rawgeti( L, LUA_REGISTRYINDEX, lot->objects.at( index ).luaVMInstance );
			lua_rawseti( L, -2, index + 1 );
		}
		
		return 1;

	}
	
	int Lot::lua_getLotObjectsByType( lua_State* L ) {
		
		BlueBear::Lot* lot = ( BlueBear::Lot* )lua_touserdata( L, lua_upvalueindex( 1 ) );
		
		// Get argument
		const char* idKey = lua_tostring( L, -1 );
		lua_pop( L, 1 );
		
		lua_newtable( L );
		
		// Push all matching objects on
		size_t objectsLength = lot->objects.size();
		size_t tableIndex = 1;
		
		for( size_t index = 0; index != objectsLength; index++ ) {
			BlueBear::Object object = lot->objects.at( index );
			
			if( strcmp( idKey, object.objType ) == 0 ) {
				lua_rawgeti( L, LUA_REGISTRYINDEX, object.luaVMInstance );
				lua_rawseti( L, -2, tableIndex );
				tableIndex++;
			}
		}
		
		return 1;
		
	}

}
