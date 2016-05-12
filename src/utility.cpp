#include "utility.hpp"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <string>
#include <cstring>
#include <string>
#include <iterator>
#include <fstream>
#include <iostream>
#include <vector>

// Not X-Platform
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

// Not X-Compiler
#include <execinfo.h>


namespace BlueBear {

	namespace Utility {

		uint16_t swap_uint16( uint16_t val ) {
			return (val << 8) | (val >> 8 );
		}

		int16_t swap_int16( int16_t val ) {
			return (val << 8) | ((val >> 8) & 0xFF);
		}

		uint32_t swap_uint32( uint32_t val ) {
			val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0xFF00FF );
			return (val << 16) | (val >> 16);
		}

		int32_t swap_int32( int32_t val ) {
			val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF );
			return (val << 16) | ((val >> 16) & 0xFFFF);
		}

		/**
		 * Dump the Lua stack out to terminal
		 */
		void stackDump( lua_State* L ) {
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

					case LUA_TNIL: /* nils */
					printf("nil");
					break;

					case LUA_TTABLE: /* table */
					printf("table");
					break;

					case LUA_TFUNCTION: /* function */
					printf("function");
					break;

				  default:  /* other values */
					printf("other type:%s", lua_typename(L, t));
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
				if ( strcmp( entry->d_name, "." ) != 0 && strcmp( entry->d_name, ".." ) != 0 ) {

					// the unix API for this is FUCKING TERRIBLE!
					if( entry->d_type == DT_DIR ) {
						directories.push_back( entry->d_name );
					} else if( entry->d_type == DT_UNKNOWN ) {
						// avoid those garbage C methods for strings
						std::string buffer = std::string( "" ) + rootSubDirectory + entry->d_name;
						const char* path = buffer.c_str();

						// black magic sourced from unix API docs
						struct stat status;
						if( stat( path, &status ) != -1 && S_ISDIR( status.st_mode ) ) {
							directories.push_back( entry->d_name );
						}
					}
				}

				entry = readdir( dir );
			}


			return directories;
		}

		void clearLuaStack( lua_State* L ) {
			lua_settop( L, 0 );
		}

		void getTableValue( lua_State* L, const char* key ) {
			// Push the desired key onto the stack
			lua_pushstring( L, key );

			// Push table[key] onto the stack
			lua_gettable( L, -2 );
		}

		void setTableIntValue( lua_State* L, const char* key, int value ) {
			lua_pushstring( L, key );
			lua_pushnumber( L, (double) value );
			lua_settable( L, -3 );
		}

		void setTableStringValue( lua_State* L, const char* key, const char* value ) {
			lua_pushstring( L, key );
			lua_pushstring( L, value );
			lua_settable( L, -3 );
		}

		void setTableFunctionValue( lua_State* L, const char* key, lua_CFunction value ) {
			lua_pushstring( L, key );
			lua_pushcfunction( L, value );
			lua_settable( L, -3 );
		}

		void doDirectories( lua_State* L, const char* directoryTitle ) {

			std::vector< std::string > directories = Utility::getSubdirectoryList( directoryTitle );

			// For each of these subdirectories, do the obj.lua file within our lua scope
			for ( std::vector< std::string >::iterator directory = directories.begin(); directory != directories.end(); directory++ ) {
				std::string scriptPath = directoryTitle + *directory + "/obj.lua";
				if( luaL_dofile( L, scriptPath.c_str() ) ) {
					printf( "Error in BlueBear object: %s\n", lua_tostring( L, -1 ) );
				}
			}

		}

		void strace() {
			void *array[10];
			size_t size;

			// get void*'s for all entries on the stack
			size = backtrace(array, 10);

			// print out all the frames to stderr
			backtrace_symbols_fd(array, size, STDERR_FILENO);
		}

		/**
		 * Tokenise a std::string based on a char value
		 */
		std::vector<std::string> split(const std::string &text, char sep) {
		  std::vector<std::string> tokens;
		  std::size_t start = 0, end = 0;
		  while ((end = text.find(sep, start)) != std::string::npos) {
		    tokens.push_back(text.substr(start, end - start));
		    start = end + 1;
		  }
		  tokens.push_back(text.substr(start));
		  return tokens;
		}

		/**
		 * Traverse and retrieve a value stored in a Lua table as a tree structure.
		 * Leaves the found value on the top of the stack; leaves nil if the value was not found.
		 * Always pops the original table.
		 */
		void getTableTreeValue( lua_State* L, std::string& treeValue ) {
			// start with <table>

			// Get tokens
			auto treeTokens = Utility::split( treeValue, '.' );

			for( const auto& token : treeTokens ) {
				// <subtable> <table> if subtable found
				Utility::getTableValue( L, token.c_str() );

				// If this is not a table, we cannot continue
				if( !lua_istable( L, -1 ) ) {
					// Pop what's on top and whatever's underneath
					// EMPTY
					lua_pop( L, 2 );

					// nil
					lua_pushnil( L );

					return;
				}

				// Pop the table under this table
				// <subtable>
				lua_remove( L, -2 );
			}

			// <desired table>
		}
	}
}
