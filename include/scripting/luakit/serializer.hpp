#ifndef LUA_SERIALIZER
#define LUA_SERIALIZER

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <jsoncpp/json/json.h>
#include <vector>
#include <string>
#include <functional>
#include <map>

namespace BlueBear {
  namespace Scripting {
    class SerializableInstance;

    namespace LuaKit {

      class Serializer {

        static const std::string TYPE_TABLE;
        static const std::string TYPE_FUNCTION;
        static const std::string TYPE_REF;
        static const std::string TYPE_CLASSID;
        static const std::string TYPE_ENVREF;

        static const std::string ENVREF_MODE_BBGLOBAL;
        static const std::string ENVREF_MODE_G;

        // These callbacks should leave the stack unmodified
        // They should accept table as the first argument on the stack
        using Callback = std::function< Json::Value() >;

        lua_State* L;
        Json::Value world;
        // Pointer-to-substitution map usable by both upvalues and tables
        std::map< std::string, Callback > substitutions;

        void createTableOnMasterList();
        void inferType( Json::Value& pair, const std::string& field );

        Json::Value createReference();
        Json::Value createClassReference();
        Json::Value createConcordiaNSReference();
        Json::Value createGReference();

        void buildSubstitutions();
        void traverseTableForSubstitutions();

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
