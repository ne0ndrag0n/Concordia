#ifndef UTILITY
#define UTILITY

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include <cstdint>
#include <fstream>
#include <vector>
#include <string>

// Not X-Compiler
#include <execinfo.h>


namespace BlueBear {
	namespace Utility {
		uint16_t swap_uint16( uint16_t val );

		int16_t swap_int16( int16_t val );

		uint32_t swap_uint32( uint32_t val );

		int32_t swap_int32( int32_t val );

		static void stackDump( lua_State* L );

		std::vector< std::string > getSubdirectoryList( const char* rootSubDirectory );

		void clearLuaStack( lua_State* L );

		void getTableValue( lua_State* L, const char* key );

		void setTableIntValue( lua_State* L, const char* key, int value );

		void setTableStringValue( lua_State* L, const char* key, const char* value );

		void setTableFunctionValue( lua_State* L, const char* key, lua_CFunction value );

		void doDirectories( lua_State* L, const char* directory );

		void strace();

	};
}


#endif
