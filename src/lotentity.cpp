#include "lotentity.hpp"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "utility.hpp"
#include <iostream>
#include <cstring>
#include <string>

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
			ok = true;

			// Grab the _cid of the LotEntity and set the public "cid" property to this value
			Utility::getTableValue( L, "_cid" );
			cid = lua_tostring( L, -1 );
			// Then, clear the value off the stack, ensuring the instance is at the top of the stack
			lua_pop( L, 1 );

			// this->luaVMInstance holds a Lua registry index to the table returned by this function
			luaVMInstance = luaL_ref( L, LUA_REGISTRYINDEX );
		} else {
			std::cout << lua_tostring( L, -1 ) << std::endl;
		}
	}

	void LotEntity::execute( unsigned int currentTick ) {
		// Push this object's table onto the API stack
		lua_rawgeti( L, LUA_REGISTRYINDEX, luaVMInstance );

		// Get the object's _sys table
		Utility::getTableValue( L, "_sys" );
		// Get the nested _sched table
		Utility::getTableValue( L, "_sched" );

		// Create the key we will need from worldTicks
		std::string tickKey = std::to_string( currentTick ) + ".0";

		// Check if tickKey has an associated value in _sched
		Utility::getTableValue( L, tickKey.c_str() );
		if( lua_istable( L, -1 ) ) {
			// There's functions that need to be executed
			// Use lua_next to get the objects that describe how to call these functions
			lua_pushnil( L );
			while( lua_next( L, -2 ) ) {
				// The Serialised Function Table (SFT) is the value, available on -1
				// The key is the index position, available on -2

				// STEP 1: Get the named function and push a closure with "self"
				// a. Push the name of the function onto the stack ( SFT[ "method" ] )
				Utility::getTableValue( L, "method" );
				// b. Save this result, then pop it
				std::string functionName = lua_tostring( L, -1 );
				lua_pop( L, 1 );
				// c. Re-push object (it is currently at -6)
				lua_pushvalue( L, -6 );
				// d. Get the function out of the object
				Utility::getTableValue( L, functionName.c_str() );
				// e. Remember the object that we just used? Re-push it as the first object argument (self)
				lua_pushvalue( L, -2 );

				// STEP 2: Push all the function's arguments
				// a. Re-grab the SFT: it should be at position -4
				lua_pushvalue( L, -4 );
				// b. Grab the "arguments" array-table
				Utility::getTableValue( L, "arguments" );
				// c. Now, time to start keeping the Lua stack cleaned up a bit; REMOVE the SFT now at position -2
				lua_remove( L, -2 );
				// d. The arguments array is now at the top of the stack. How many are in it?
				int argumentsLength = lua_rawlen( L, -1 );



				// Value has to be removed from the stack after we're done,
				// leaving the key at -1 and the table at -2 (see how the loop restarts?)
				lua_pop( L, 1 );
			}
		} else {
			// There's no functions marked for this tick
			// Start popping all this crap off the stack
			lua_pop( L, 3 );
		}

		// Pop the object table
		lua_pop( L, 1 );
	}
}
