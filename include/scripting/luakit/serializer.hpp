#ifndef LUA_SERIALIZER
#define LUA_SERIALIZER

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <jsoncpp/json/json.h>
#include <vector>

namespace BlueBear {
  namespace Scripting {
    class SerializableInstance;

    namespace LuaKit {

      class Serializer {

        lua_State* L;
        int currentRecursionLevel;

        void traverseTable( Json::Value& masterCollection );

      public:
        Serializer( lua_State* L );

        /**
         * Save the world to JSON value
         */
        Json::Value saveWorld( std::vector< SerializableInstance >& objects );
      };

    }
  }
}


#endif
