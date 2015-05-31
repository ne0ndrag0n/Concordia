#include "bluebear.hpp"
#include "bbtypes.hpp"
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

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
	
	void Engine::loadLot( const char* lotPath ) {
		
	}
	
	/**
	 * Where the magic happens 
	 */
	void Engine::objectLoop() {
		
	}
	
	namespace Utility {
		
		/**
		 * Dump the Lua stack out to terminal
		 */
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
		
		/**
		 * @noxplatform
		 * 
		 * Gets a collection of subdirectories for the given directory
		 */
		std::vector< std::string > getSubdirectoryList( const char* rootSubDirectory ) {
			std::vector< std::string > directories;
			
			DIR *dir = opendir( rootSubDirectory );

			struct dirent* entry = readdir( dir );

			while ( entry != NULL ) {
				if ( entry->d_type == DT_DIR && strcmp( entry->d_name, "." ) != 0 && strcmp( entry->d_name, ".." ) != 0 )
					directories.push_back( entry->d_name );
					
				entry = readdir( dir );
			}

			
			return directories;
		}
		
		/**
		 * Used for testing purposes 
		 */
		void simpleStringPrinter( std::string string ) {
			std::cerr << string << std::endl; 
		}
	}
}
