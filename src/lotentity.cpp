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
	LotEntity::LotEntity( lua_State* L, const char* idKey, char* popPackage, int popSize, BlueBear::LotEntityType lotEntityType ) : objType( idKey ) {
		
		// Store pointer to Luasphere on this object
		this->L = L;
		
		// Store lot entity type
		this->lotEntityType = lotEntityType;
		
		// Get fresh start with the Lua stack
		Utility::clearLuaStack( L );
		
		// Push _bblib onto Lua API stack
		lua_getglobal( L, "_bblib" );
		
		// Get instantiate_pop method
		Utility::getTableValue( L, "instantiate_pop" );
		
		// Push _bbobject key and POP package. Push whether or not this object is a plain object or a player character
		lua_pushstring( L, idKey );
		lua_pushlstring( L, popPackage, popSize );
		lua_pushnumber( L, static_cast< lua_Number >( lotEntityType ) + 1 );

		// Call instantiate_pop
		if( lua_pcall( L, 3, 1, 0 ) == 0 ) {
			this->ok = true;
			
			// This will return a reference to the entry in _bblib - Pop and use this to store a reference to this function in this->luaVMInstance
			this->luaVMInstance = luaL_ref( L, LUA_REGISTRYINDEX );
		} else {
			std::cout << lua_tostring( L, -1 ) << "\n";
		}
		
	}
	
	void LotEntity::execute( unsigned int worldTicks ) {
		
		if( this->ok == false ) {
			return;
		}
		
		unsigned int nextTickSchedule;
		
		// Clear the API stack of the Luasphere
		Utility::clearLuaStack( this->L );

		// Push this object's table onto the API stack
		lua_rawgeti( this->L, LUA_REGISTRYINDEX, this->luaVMInstance );
		
		// First, we need to push a reference to the _sys table
		Utility::getTableValue( this->L, "_sys" );
		
		// Next, push the value of _sched within _sys
		Utility::getTableValue( this->L, "_sched" );
		
		// Extract and pop int (and _sys table) from top of stack
		nextTickSchedule = lua_tonumber( this->L, -1 );
		lua_pop( this->L, 2 );
		
		// Execute only if the amount of ticks is just right (worldTicks >= nextTickSchedule)
		if( worldTicks >= nextTickSchedule ) {

			std::cout << "Running new iteration for luaVMInstance " << this->luaVMInstance << ", current worldTicks is " << worldTicks << " and this object's nextTickSchedule is " << nextTickSchedule << "\n";
			
			// Push the object's "main" method
			Utility::getTableValue( this->L, "main" );
			
			// Re-push table onto stack as argument 
			lua_pushvalue( this->L, -2 );
			
			// Get lot table and push this as the second argument
			lua_rawgeti( this->L, LUA_REGISTRYINDEX, this->lotTableRef );
			
			// Run function
			if( lua_pcall( this->L, 2, 1, 0 ) != 0 ) {
				std::cerr << lua_tostring( this->L, -1 ) << "\n";
				this->ok = false;
			}
			
			// This function returns a tick amount. The next execution is current world ticks + this amount
			// Set this object's _sys._sched to worldTicks + nextTickSchedule
			nextTickSchedule = lua_tonumber( this->L, -1 );
			lua_pop( this->L, 1 );
			
			// The function and its arguments should be popped, leaving the object itself
			// Get the _sys table
			Utility::getTableValue( this->L, "_sys" );

			// Set the _sched value
			Utility::setTableIntValue( this->L, "_sched", worldTicks + nextTickSchedule );
		}
	}
}
