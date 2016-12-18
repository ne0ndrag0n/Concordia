#include "tools/utility.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <string>
#include <cstring>
#include <json/json.h>
#include <string>
#include <iterator>
#include <fstream>
#include <vector>
#include <sstream>

// Not X-Platform
#ifndef _WIN32
	#include <dirent.h>
	#include <unistd.h>
	#include <sys/stat.h>
#endif

namespace BlueBear {
	namespace Tools {
		/**
		 * Dump the Lua stack out to terminal
		 */
		void Utility::stackDump( lua_State* L ) {
			  int i;
			  int top = lua_gettop(L);
			  for (i = 1; i <= top; i++) {  /* repeat for each level */
					Utility::stackDumpAt( L, -i );
					printf("  ");  /* put a separator */
			  }

			  printf("\n");  /* end the listing */
		}

		void Utility::stackDumpAt( lua_State* L, int pos ) {
			int t = lua_type(L, pos);
			switch (t) {

				case LUA_TSTRING:  /* strings */
				printf("`%s'", lua_tostring(L, pos));
				break;

				case LUA_TBOOLEAN:  /* booleans */
				printf(lua_toboolean(L, pos) ? "true" : "false");
				break;

				case LUA_TNUMBER:  /* numbers */
				printf("%g", lua_tonumber(L, pos));
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
		}

		/**
		 * @noxplatform
		 *
		 * Gets a collection of subdirectories for the given directory
		 */
		std::vector< std::string > Utility::getSubdirectoryList( const char* rootSubDirectory ) {
			std::vector< std::string > directories;

			#ifndef _WIN32
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
			#else
				// STUB !!
				// TODO: Windows directory listing
			#endif

			closedir( dir );
			return directories;
		}

		/**
		 * Returns not only a list of subdirectories, but also files included. This really should be combined
		 * with the older function above.
		 */
		std::vector< Utility::DirectoryEntry > Utility::getFileList( const std::string& parent ) {
			std::vector< Utility::DirectoryEntry > files;

			#ifndef _WIN32

			DIR* dir = opendir( parent.c_str() );
			struct dirent* entry = readdir( dir );

			while( entry != NULL ) {
				// exclude current directory and parent directory
				if( strcmp( entry->d_name, "." ) != 0 && strcmp( entry->d_name, ".." ) != 0 ) {

					Utility::FilesystemType fsType;
					switch( entry->d_type ) {
						case DT_DIR:
							fsType = Utility::FilesystemType::DIRECTORY;
							break;
						case DT_REG:
							fsType = Utility::FilesystemType::FILE;
							break;
						default:
							// Set it unknown for now. If you get problems, a check will need to be done similar above
							// because some network-mounted filesystems may report DT_UNKNOWN as the file type.
							//
							// TODO: Enter this as a task now! Symlinks will fall back to FileystemType::UNKNOWN!!
							break;
					}

					files.push_back( Utility::DirectoryEntry{ fsType, std::string( entry->d_name ) } );
				}

				entry = readdir( dir );
			}
			#else
				// STUB !!
				// TODO: Windows file listing
			#endif

			closedir( dir );
			return files;
		}

		/**
		 * Create the file list for lua. Lua changes things a bit:
		 * The returned array will be a table of tables, each table containing two fields
		 * type -> "directory" or "file". Unknowns will not be added.
		 * name -> Straight rip of the name from the Utility::DirectoryEntry
		 */
		int Utility::lua_getFileList( lua_State* L ) {

			// String shall be the first argument
			if( lua_isstring( L, -1 ) ) {
				// string
				std::string path = lua_tostring( L, -1 );
				auto fileList = Utility::getFileList( path );

				// EMPTY
				lua_pop( L, 1 );

				// array
				lua_newtable( L );

				int currentIndex = 1;
				for( auto directoryEntry : fileList ) {

					if( directoryEntry.type != Utility::FilesystemType::UNKNOWN ) {

						// entrytable array
						lua_newtable( L );

						// "type" entrytable array
						lua_pushstring( L, "type" );

						// type "type" entrytable array
						switch( directoryEntry.type ) {
							case Utility::FilesystemType::DIRECTORY:
								lua_pushstring( L, Utility::DIRECTORY_STRING );
								break;
							case Utility::FilesystemType::FILE:
								lua_pushstring( L, Utility::FILE_STRING );
								break;
						}

						// entrytable array
						lua_settable( L, -3 );

						// "name" entrytable array
						lua_pushstring( L, "name" );

						// name "name" entrytable array
						lua_pushstring( L, directoryEntry.name.c_str() );

						// entrytable array
						lua_settable( L, -3 );

						// array
						lua_rawseti( L, -2, currentIndex++ );

					}
				}
			}

			// array
			return 1;
		}

		void Utility::clearLuaStack( lua_State* L ) {
			lua_settop( L, 0 );
		}

		void Utility::getTableValue( lua_State* L, const char* key ) {
			// Push the desired key onto the stack
			lua_pushstring( L, key );

			// Push table[key] onto the stack
			lua_gettable( L, -2 );
		}

		void Utility::setTableIntValue( lua_State* L, const char* key, int value ) {
			lua_pushstring( L, key );
			lua_pushnumber( L, (double) value );
			lua_settable( L, -3 );
		}

		void Utility::setTableStringValue( lua_State* L, const char* key, const char* value ) {
			lua_pushstring( L, key );
			lua_pushstring( L, value );
			lua_settable( L, -3 );
		}

		void Utility::setTableFunctionValue( lua_State* L, const char* key, lua_CFunction value ) {
			lua_pushstring( L, key );
			lua_pushcfunction( L, value );
			lua_settable( L, -3 );
		}

		/**
		 * Tokenise a std::string based on a char value
		 */
		std::vector<std::string> Utility::split(const std::string &text, char sep) {
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
		 * C++ doesn't have one of these built into the damn string class.
		 */
		std::string Utility::join( const std::vector< std::string >& strings, const std::string& token ) {
			std::string result;

			unsigned int size = strings.size();
			for( unsigned int i = 0; i != size; i++ ) {
				result += strings[ i ];
				if ( i != size - 1 ) {
					result += token;
				}
			}

			return result;
		}

		/**
		 * Traverse and retrieve a value stored in a Lua table as a tree structure.
		 * Leaves the found value on the top of the stack; leaves nil if the value was not found.
		 * Always pops the original table.
		 */
		void Utility::getTableTreeValue( lua_State* L, const std::string& treeValue ) {
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

		/**
		 * Determines if Json::Value is an RLE object. An RLE object is an object, contains a numeric "run" field, and a defined "value" field.
		 */
		bool Utility::isRLEObject( Json::Value& value ) {
			return value.isObject() &&
				value.get( "run", Json::Value::null ).isInt() &&
				!( value.get( "value", Json::Value::null ).isNull() );
		}

		std::string Utility::stringLtrim( std::string& s ) {
			return s.substr( s.find_first_not_of( " \t\f\v\n\r" ) );
		}

		std::string Utility::stringRtrim( std::string& s ) {
			return s.erase( s.find_last_not_of( " \t\f\v\n\r" ) + 1 );
		}

		std::string Utility::stringTrim( std::string& s ) {
			std::string rTrimmed = Utility::stringRtrim( s );

			return Utility::stringLtrim( rTrimmed );
		}

		std::string Utility::pointerToString( const void* pointer ) {
			std::stringstream ss;
			ss << pointer;
			return ss.str();
		}
	}
}
