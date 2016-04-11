#include "bbtypes.hpp"
#include "lotentity.hpp"
#include "lot.hpp"
#include "utility.hpp"
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
		size_t arrIndex = 1;
		for( size_t index = 0; index != objectsLength; index++ ) {
			//if( lot->objects[ index ].lotEntityType == BlueBear::LotEntityType::TYPE_OBJECT ) {
			//	lua_rawgeti( L, LUA_REGISTRYINDEX, lot->objects[ index ].luaVMInstance );
			//	lua_rawseti( L, -2, arrIndex++ );
			//}
		}

		return 1;

	}

	int Lot::lua_getLotObjectsByType( lua_State* L ) {

		BlueBear::Lot* lot = ( BlueBear::Lot* )lua_touserdata( L, lua_upvalueindex( 1 ) );

		// Get argument (the class we are looking for) and remove it from the stack
		// Copy using modern C++ string methods into archaic, unsafe C-string format used by Lua API
		std::string keystring( lua_tostring( L, -1 ) );
		const char* idKey = keystring.c_str();
		lua_pop( L, 1 );

		// This table will be the array of matching lot objects
		lua_newtable( L );

		// Start at index number 1 - Lua arrays (tables) start at 1
		size_t tableIndex = 1;

		// Iterate through each object on the lot, checking to see if each is an instance of "idKey"
		for( BlueBear::LotEntity object : lot->objects ) {
			// Push bluebear global
			lua_getglobal( L, "bluebear" );

			// Push instance_of utility function
			Utility::getTableValue( L, "instance_of" );

			// Push the two arguments: identifier, and instance
			lua_pushstring( L, idKey );
			lua_rawgeti( L, LUA_REGISTRYINDEX, object.luaVMInstance );

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
					lua_rawgeti( L, LUA_REGISTRYINDEX, object.luaVMInstance );
					// Push it onto the table on our stack
					lua_rawseti( L, -2, tableIndex++ );
				}
			}
		}

		return 1;

	}
}
