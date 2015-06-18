#include "bbtypes.hpp"
#include "lot.hpp"
#include <vector>
#include <cstring>
#include <string>
#include <iostream>

extern "C" {
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}


namespace BlueBear {
	
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
			lua_rawgeti( L, LUA_REGISTRYINDEX, lot->objects[ index ].luaVMInstance );
			lua_rawseti( L, -2, index + 1 );
		}
		
		return 1;

	}
	
	int Lot::lua_getLotObjectsByType( lua_State* L ) {
		
		BlueBear::Lot* lot = ( BlueBear::Lot* )lua_touserdata( L, lua_upvalueindex( 1 ) );
				
		// Get argument
		std::string idKey( lua_tostring( L, -1 ) );
		lua_pop( L, 1 );

		lua_newtable( L );
		
		// Push all matching objects on
		size_t objectsLength = lot->objects.size();
		size_t tableIndex = 1;
		
		for( size_t index = 0; index != objectsLength; index++ ) {
			BlueBear::Object object = lot->objects[ index ];
			
			if( idKey == object.objType ) {
				//std::cout << object.objType << "\n";
				lua_rawgeti( L, LUA_REGISTRYINDEX, object.luaVMInstance );
				lua_rawseti( L, -2, tableIndex );
				tableIndex++;
			}
		}
		
		return 1;
		
	}
}
