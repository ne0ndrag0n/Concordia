#ifndef LUA_SERIALIZER
#define LUA_SERIALIZER

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <jsoncpp/json/json.h>
#include <vector>
#include <string>

namespace BlueBear {
  namespace Scripting {
    class SerializableInstance;

    namespace LuaKit {

      class Serializer {

        static const std::string TYPE_TABLE;
        static const std::string TYPE_REF;
        static const std::string TYPE_CLASSID;

        static const std::string FIELD_CLASS;

        lua_State* L;
        Json::Value world;

        void isClassTable( bool keyIsClass );

        void createTableOnMasterList();
        void inferType( Json::Value& pair, const std::string& field, bool keyIsClass = false );

        Json::Value createReference();
        Json::Value createClassReference();

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
