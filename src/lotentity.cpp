#include "lotentity.hpp"
#include "utility.hpp"
#include "bbtypes.hpp"
#include <iostream>
#include <cstring>
#include <string>

extern "C" {
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

namespace BlueBear {

	/**
	 * Every BlueBear::LotEntity is tied to its Lua instance in the _lotinsts table
	 */
	LotEntity::LotEntity( lua_State* L, std::string& classID, std::string& instance ) {
		// Store pointer to Luasphere on this object
		this->L = L;

		// Store classID in this->classID
		this->classID = classID;

		// Get fresh start with the Lua stack
		Utility::clearLuaStack( L );

		// Push bluebear onto Lua API stack
		lua_getglobal( L, "bluebear" );

		// Get new_instance_from_file method. This method will not only create a new instance, but also deserialise
		// the object provided, creating an instance that should be identical to what was saved previously
		Utility::getTableValue( L, "new_instance_from_file" );

		// Push identifier and instance string
		lua_pushstring( L, classID.c_str() );
		lua_pushstring( L, instance.c_str() );

		// Call new_instance_from_file
		if( lua_pcall( L, 2, 1, 0 ) == 0 ) {
			// Mark this LotEntity as OK to run
			this->ok = true;

			// Grab the _cid of the LotEntity and set the public "cid" property to this value
			Utility::getTableValue( L, "_cid" );
			this->cid = lua_tostring( L, -1 );
			// Then, clear the value off the stack, ensuring the instance is at the top of the stack
			lua_pop( L, 1 );

			// this->luaVMInstance holds a Lua registry index to the table returned by this function
			this->luaVMInstance = luaL_ref( L, LUA_REGISTRYINDEX );
		} else {
			std::cout << lua_tostring( L, -1 ) << std::endl;
		}
	}

	void LotEntity::execute() {
		// Push this object's table onto the API stack
		lua_rawgeti( this->L, LUA_REGISTRYINDEX, this->luaVMInstance );

		// Push the table's _run method
		Utility::getTableValue( this->L, "_run" );

		// Push the table itself
		lua_pushvalue( this->L, -2 );

		// Run the _run method
		if( lua_pcall( this->L, 1, 0, 0 ) != 0 ) {
			// If there is an error, print error to console and pop it
			std::cerr << lua_tostring( this->L, -1 ) << std::endl;
			lua_pop( this->L, 1 );

			// Mark object as bad - don't run it anymore!
			this->ok = false;
		}

		// Pop the object table
		lua_pop( this->L, 1 );
	}

	int LotEntity::lua_getLotEntityObject( lua_State* L ) {
		BlueBear::LotEntity* lotEntity = ( BlueBear::LotEntity* )lua_touserdata( L, lua_upvalueindex( 1 ) );


	}
}
