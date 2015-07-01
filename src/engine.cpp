#include "bbtypes.hpp"
#include "utility.hpp"
#include "lotentity.hpp"
#include "lot.hpp"
#include "engine.hpp"
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
		// Start with templates
		Utility::doDirectories( L, BLUEBEAR_TEMPLATES_DIRECTORY );
		// Now do the objects
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
		std::ifstream lot( lotPath, std::ifstream::binary );
		
		if( lot.is_open() && lot.good() ) {
			BlueBear::BBLTLotHeader lotHeader;
			lot.read( reinterpret_cast< char* >( &lotHeader ), 10 );
			
			if( Utility::swap_uint32( lotHeader.magicID ) == BLUEBEAR_LOT_MAGIC_ID ) {
				
				// Instantiate the lot
				this->currentLot = new BlueBear::Lot(
					lotHeader.lotX,
					lotHeader.lotY,
					lotHeader.numStories,
					lotHeader.undergroundStories,
					static_cast< BlueBear::TerrainType >( lotHeader.terrainType )
				);

				
				// Create one lot table for the Luasphere - contains functions that we call on this->currentLot to do things like get other objects on the lot and trigger events
				lotTableRef = this->createLotTable( this->currentLot );
				
				// This doesn't seem to work properly as part of the struct. don't know why, so get it separately instead
				uint32_t lotTime = Utility::getuint32_t( &lot );
				this->worldTicks = static_cast< unsigned int >( lotTime );
				
				// Load length of LotEntity Definition Table (ODT)
				uint32_t odtSize = Utility::getuint32_t( &lot );

				// Read in the ODT
				char odt[ odtSize ] = { 0 };
				lot.read( odt, static_cast< int >( odtSize ) );
				std::vector< BlueBear::OdtEntry > objectIDs;
				
				// Create ODT table
				char* odtPtr = odt;
				size_t index = 0;
				while ( index < static_cast< size_t >( odtSize ) ) {
					objectIDs.push_back( { BlueBear::LotEntityType::TYPE_OBJECT, std::string( odtPtr ) } );
					index = index + objectIDs.back().typeKey.size() + 1;
					odtPtr += objectIDs.back().typeKey.size() + 1;
				}
				
				// Verify each object exists
				if( !( this->verifyODT( objectIDs ) ) ) {
					std::cout << "An object in this lot does not exist in the global ODT!" << std::endl;
					return false;
				}
				
				// Get size of the OIT
				uint32_t oitSize = Utility::getuint32_t( &lot );
				
				this->currentLot->objects.clear();

				// Create BBLotEntitys
				for( size_t i = 0; i != oitSize; i++ ) {
					// Each POP begins with the category flag
					uint8_t categoryFlag = 0;
					
					// Each POP also begins with an index of the item in the OIT
					uint16_t odtIndex = Utility::getuint16_t( &lot );
					
					// This is the size of the POP
					uint16_t popSize = Utility::getuint16_t( &lot );
					
					// Create POP
					char pop[ popSize ] = { 0 };
					lot.read( pop, static_cast< int >( popSize ) );
					
					// Add object to Engine objects vector
					// BlueBear::LotEntity instances are wrappers around the Lua instances of the object
					BlueBear::LotEntity obj( this->L, objectIDs.at( odtIndex ).typeKey.c_str(), pop, popSize, static_cast< BlueBear::LotEntityType >( categoryFlag ) );
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
	 * Verify that the LotEntity Definition Table entries have corresponding objects in the Luasphere
	 * 
	 * @param		{std::vector< std::string >}	odt		The object definition table
	 */
	bool Engine::verifyODT( std::vector< BlueBear::OdtEntry > odt ) {
		Utility::clearLuaStack( this->L );
		
		// Push _classes onto Lua API stack
		lua_getglobal( this->L, "_classes" );
		
		// Get "objects" within classes
		Utility::getTableValue( this->L, "objects" );

		for ( std::vector< BlueBear::OdtEntry >::iterator odtEntry = odt.begin(); odtEntry != odt.end(); odtEntry++ ) {
			Utility::getTableValue( this->L, odtEntry->typeKey.c_str() );
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
			for( std::vector< BlueBear::LotEntity >::iterator object = this->currentLot->objects.begin(); object != this->currentLot->objects.end(); object++ ) {	
				object->execute( this->worldTicks );
			}
		}
		
		std::cout << std::endl;
	}

}
