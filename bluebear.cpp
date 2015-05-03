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
	
	BBObject::BBObject( const char* fileName ) {
		this->fileName = fileName;
		this->fileContents = NULL;

		std::ifstream fileStream( fileName );
		if( fileStream.good() ) {
			std::string content( 
				( std::istreambuf_iterator< char >( fileStream ) ),
				( std::istreambuf_iterator< char >()    	     ) 
			);
			this->fileContents = content.c_str();
		}
		
		fileStream.close();
	}
	
	
	bool BBObject::good() {
		return this->fileContents != NULL;
	}
	
	const char* BBObject::getFileContents() {
		return this->fileContents;
	}

	BBObject Engine::getObjectFromFile( const char* fileName ) {
		BlueBear::BBObject bbObject( fileName );
		
		if( bbObject.good() ) {
			this->objects.push_back( bbObject );
			return bbObject;
		}
		
		return NULL;
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
