#include "bluebear.hpp"
#include <cstdio>
#include <cstdarg>
#include <fstream>
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
		
		return true;
	}
	
	namespace Utility {
		static void stackDump( lua_State* L ) {
			  int i;
			  int top = lua_gettop(L);
			  for (i = 1; i <= top; i++) {  /* repeat for each level */
				int t = lua_type(L, i);
				switch (t) {
			
				  case LUA_TSTRING:  /* strings */
					printf("`%s'", lua_tostring(L, i));
					break;
			
				  case LUA_TBOOLEAN:  /* booleans */
					printf(lua_toboolean(L, i) ? "true" : "false");
					break;
			
				  case LUA_TNUMBER:  /* numbers */
					printf("%g", lua_tonumber(L, i));
					break;
			
				  default:  /* other values */
					printf("%s", lua_typename(L, t));
					break;
			
				}
				printf("  ");  /* put a separator */
			  }
			  printf("\n");  /* end the listing */
		}
	}
}
