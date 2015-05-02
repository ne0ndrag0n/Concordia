#include "bluebear.hpp"
#include <iostream>
#include <cstdio>

extern "C" {
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

int main() {
	
	const char* script =
		"print(hello)\n"
		"function run_object(x)\n"
		"    print(\"x^2 in lua: \" .. x^2)\n"
		"    return x^2\n"
		"end";
	
	lua_State* L = luaL_newstate();
	luaL_openlibs( L );
	
	if ( luaL_loadstring( L, script ) ) {
		fprintf( stderr, "Couldn't process script: %s\n", lua_tostring( L, -1 ) );
		return 1;
	}
	
	lua_pushstring( L, "Hello, BlueBear!!" );
	lua_setglobal( L, "hello" );
	
	if ( lua_pcall( L, 0, LUA_MULTRET, 0 ) ) {
		fprintf( stderr, "Failed to run script: %s\n", lua_tostring( L, -1 ) );
        return 1;
	}
	
	lua_close( L );
	return 0;
}
