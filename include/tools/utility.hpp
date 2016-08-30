#ifndef UTILITY
#define UTILITY

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <cstdint>
#include <jsoncpp/json/json.h>
#include <cstddef>
#include <fstream>
#include <vector>
#include <string>

namespace BlueBear {
	namespace Tools {
		class Utility {

			static constexpr const char* UNKNOWN_STRING = "unknown";
			static constexpr const char* DIRECTORY_STRING = "directory";
			static constexpr const char* FILE_STRING = "file";

			public:
				enum class FilesystemType : int { UNKNOWN, DIRECTORY, FILE };
				struct DirectoryEntry {
					FilesystemType type;
					std::string name;
				};

				static void stackDump( lua_State* L );

				static void stackDumpAt( lua_State* L, int pos );

				static std::vector< std::string > getSubdirectoryList( const char* rootSubDirectory );

				static std::vector< DirectoryEntry > getFileList( const std::string& parent );

				static int lua_getFileList( lua_State* L );

				static void clearLuaStack( lua_State* L );

				static void getTableValue( lua_State* L, const char* key );

				static void setTableIntValue( lua_State* L, const char* key, int value );

				static void setTableStringValue( lua_State* L, const char* key, const char* value );

				static void setTableFunctionValue( lua_State* L, const char* key, lua_CFunction value );

				static std::vector<std::string> split(const std::string &text, char sep);

				static void getTableTreeValue( lua_State* L, const std::string& treeValue );

				static bool isRLEObject( Json::Value& value );
		};
	}
}


#endif
