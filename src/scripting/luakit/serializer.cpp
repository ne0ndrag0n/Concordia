#include "scripting/luakit/serializer.hpp"
#include "scripting/serializableinstance.hpp"
#include "tools/utility.hpp"
#include "log.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <jsoncpp/json/json.h>
#include <string>

namespace BlueBear {
  namespace Scripting {
    namespace LuaKit {

      const std::string Serializer::TYPE_TABLE = "table";
      const std::string Serializer::TYPE_REF = "ref";
      const std::string Serializer::TYPE_CLASSID = "class";

      const std::string Serializer::FIELD_CLASS = "class";

      Serializer::Serializer( lua_State* L ) : L( L ) {}

      /**
       * Using the Engine-tracked index of system.entity.base objects as a starting point, save the current state of the Lua world.
       */
      Json::Value Serializer::saveWorld( std::vector< SerializableInstance >& objects ) {
        world = Json::Value( Json::objectValue );

        // STOP the garbage collector so pointer references remain intact as we operate
        // Lua currently doesn't move items around as part of garbage collection (I think) but relying
        // on it is still undefined behaviour
        lua_gc( L, LUA_GCSTOP, 0 );

        for( SerializableInstance& instance : objects ) {
          // table
          lua_rawgeti( L, LUA_REGISTRYINDEX, instance.luaVMInstance );

          // EMPTY
          createTableOnMasterList();
        }

        // Restart the garbage collector, and give it a good cycle
        lua_gc( L, LUA_GCRESTART, 0 );
        lua_gc( L, LUA_GCCOLLECT, 0 );

        return world;
      }

      /**
       * Print the contents of a table (for now...)
       *
       * STACK ARGS: table
       * RETURNS: none
       */
      void Serializer::createTableOnMasterList() {

        // As soon as a table is found, go ahead and throw it on the pile
        std::string this_table( Tools::Utility::pointerToString( lua_topointer( L, -1 ) ) );
        Json::Value& item = world[ this_table ] = Json::Value( Json::arrayValue );

        lua_pushnil( L ); // nil table

        // Iterate over the contents of this table
        while( lua_next( L, -2 ) != 0 ) { // value key table

          // { "key": <objtype>, "value": <objtype> }
          Json::Value pair = Json::Value( Json::objectValue );

          lua_pushvalue( L, -2 ); // key value key table
          bool keyIsClass = false;
          if( lua_isstring( L, -1 ) ) {
            keyIsClass = std::string( lua_tostring( L, -1 ) ) == FIELD_CLASS;
          }
          lua_pop( L, 1 ); // value key table

          inferType( pair, "value", keyIsClass ); // key table

          lua_pushvalue( L, -1 ); // key key table
          inferType( pair, "key" ); // key table

          item.append( pair );
        } // table


        lua_pop( L, 1 ); // EMPTY
      }

      /**
       * Infer the type, load references if necessary, and place it into pair[field]
       *
       * STACK ARGS: (any lua type to infer)
       * RETURNS: EMPTY
       */
      void Serializer::inferType( Json::Value& pair, const std::string& field, bool keyIsClass ) {

        // Get type
        const char* type = lua_typename( L, lua_type( L, -1 ) );

        // Different action based on type
        // make sure our hash function is good here
        switch( Tools::Utility::hash( type ) ) {
          case Tools::Utility::hash( "string" ):
            lua_pushvalue( L, -1 ); // string string
            pair[ field ] = lua_tostring( L, -1 );
            lua_pop( L, 1 ); // string
            break;
          case Tools::Utility::hash( "number" ):
            pair[ field ] = lua_tonumber( L, -1 );
            break;
          case Tools::Utility::hash( "boolean" ):
            pair[ field ] = lua_toboolean( L, -1 ) ? true : false;
            break;
          case Tools::Utility::hash( "table" ):
            {
              isClassTable( keyIsClass ); // ("classID" || nil) table

              if( lua_isstring( L, -1 ) ) {

                pair[ field ] = createClassReference(); // table

              } else {
                // Normal table

                lua_pop( L, 1 ); // table

                // If this table was already found in world, then simply create the reference. Else, expose table to world, then create the reference.
                std::string worldPointer = Tools::Utility::pointerToString( lua_topointer( L, -1 ) );

                if( !world.isMember( worldPointer ) ) {
                  // Need to create the table
                  // Copy the stack value as createTableOnMasterList will remove it
                  lua_pushvalue( L, -1 ); // table table
                  createTableOnMasterList(); // table
                }

                pair[ field ] = createReference();
              }
            }
            break;
          case Tools::Utility::hash( "function" ):
            // TODO: The big function case
            break;
          default:
            Log::getInstance().warn( "LuaKit::Serializer::inferType", "Invalid type: " + std::string( type ) + ", substituting null." );
          case Tools::Utility::hash( "nil" ):
              pair[ field ] = Json::Value::null;
        }

        lua_pop( L, 1 ); // EMPTY
      }

      /**
       * Return a JSON value representing a reference to a floating world object. This function is only defined for the types specified below.
       *
       * STACK ARGS: table OR function
       * (Stack is unmodified after call)
       */
      Json::Value Serializer::createReference() {
        Json::Value val( Json::objectValue );

        val[ "type" ] = Serializer::TYPE_REF;
        val[ "ptr" ] = Tools::Utility::pointerToString( lua_topointer( L, -1 ) );

        return val;
      }

      /**
       * Return a JSON file representing a reference to a class.
       *
       * STACK ARGS: "classID"
       * RETURNS: EMPTY
       */
      Json::Value Serializer::createClassReference() {
        Json::Value val( Json::objectValue );

        val[ "type" ] = Serializer::TYPE_CLASSID;
        lua_pushvalue( L, -1 ); // string string
        val[ "id" ] = lua_tostring( L, -1 );

        lua_pop( L, 2 ); // EMPTY

        return val;
      }

      /**
       * Determine if the table is a class table. A class table has a table named "class" within, with a field named "name" further within.
       *
       * STACK ARGS: table
       * RETURNS: ("string" if class, nil if not a class) table
       */
      void Serializer::isClassTable( bool keyIsClass ) {

          if( keyIsClass && lua_istable( L, -1 ) ) {
            lua_pushstring( L, "name" ); // "name" table
            lua_gettable( L, -2 ); // "class.name" table

            if( lua_isstring( L, -1 ) ) {
              return;
            } else {
              lua_pop( L, 1 ); // table
            }

          }

          lua_pushnil( L ); // nil table
      }
    }
  }
}
