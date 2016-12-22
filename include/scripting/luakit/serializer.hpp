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
        static const std::string TYPE_ITABLE;
        static const std::string TYPE_FUNCTION;
        static const std::string TYPE_SFUNCTION;
        static const std::string TYPE_REF;
        static const std::string TYPE_CLASSID;
        static const std::string TYPE_ENVREF;

        static const std::string ENVREF_MODE_BBGLOBAL;
        static const std::string ENVREF_MODE_G;

        // These callbacks should leave the stack unmodified
        // They should accept table/function as the first argument on the stack
        using Callback = std::function< Json::Value() >;

        lua_State* L;
        Json::Value world;
        // Pointer-to-substitution map usable by both upvalues and tables
        std::map< std::string, Callback > substitutions;
        // When loading a lot from disk, use these maps to track top-level objects
        std::map< std::string, int > globalEntities;
        std::map< std::string, int > globalInstanceEntities;

        // --- saving ---
        void createTableOnMasterList();
        void createFunctionOnMasterList();
        void inferType( Json::Value& pair, const std::string& field );

        Json::Value createReference();
        Json::Value createClassReference();
        Json::Value createConcordiaNSReference();
        Json::Value createGReference();

        void buildSubstitutions();
        void traverseTableForSubstitutions();

        bool canCreateSfunction();

        void getUpvalueByName( const std::string& name );
        void addUpvalues( Json::Value& funcType );

        // --- loading ---
        int createGlobalItem( const std::string& addressKey );
        int createTable( const std::string& addressKey, Json::Value& tableDefinition );
        void getReference( const std::string& addressKey );
        void inferTypeFromJSON( Json::Value& objectToken );
        bool globalItemExists( const std::string& addressKey );

      public:
        Serializer( lua_State* L );

        /**
         * Save the world to JSON value
         */
        Json::Value saveWorld( std::vector< SerializableInstance >& objects );
        std::vector< SerializableInstance > loadWorld( Json::Value& engineDefinition );
      };

    }
  }
}


#endif
