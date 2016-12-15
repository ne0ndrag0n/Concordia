#include "scripting/luakit/serializer.hpp"
#include "scripting/serializableinstance.hpp"
#include "tools/utility.hpp"
#include "log.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <jsoncpp/json/json.h>

namespace BlueBear {
  namespace Scripting {
    namespace LuaKit {

      Serializer::Serializer( lua_State* L ) : L( L ), currentRecursionLevel( 0 ) {}

      /**
       * Using the Engine-tracked index of system.entity.base objects as a starting point, save the current state of the Lua world.
       */
      Json::Value Serializer::saveWorld( std::vector< SerializableInstance >& objects ) {
        Json::Value world;

        // STOP the garbage collector so pointer references remain intact as we operate
        // Lua currently doesn't move items around as part of garbage collection (I think) but relying
        // on it is still undefined behaviour
        lua_gc( L, LUA_GCSTOP, 0 );

        currentRecursionLevel = 0;

        for( SerializableInstance& instance : objects ) {
          // table
          lua_rawgeti( L, LUA_REGISTRYINDEX, instance.luaVMInstance );

          // EMPTY
          traverseTable( world );
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
      void Serializer::traverseTable( Json::Value& masterCollection ) {

        // As soon as a table is found, go ahead and throw it on the pile
        std::string this_table( Tools::Utility::pointerToString( lua_topointer( L, -1 ) ) );
        masterCollection[ this_table ] = Json::Value();

        lua_pushnil( L ); // nil table

        // Iterate over the contents of this table
        while( lua_next( L, -2 ) != 0 ) { // value key table

          // Step 1: native lua form of tostring on key
          lua_getglobal( L, "tostring" ); // <tostring> value key table
          lua_pushvalue( L, -3 ); // key <tostring> value key table

          if( lua_pcall( L, 1, 1, 0 ) ) { // error value key table
            Log::getInstance().error( "LuaKit::Serializer::printTableContents", lua_tostring( L, -1 ) );
            lua_pop( L, 4 ); // EMPTY
            return;
          } // "string representation" value key table

          std::string key( lua_tostring( L, -1 ) );

          lua_pop( L, 1 ); // value key table

          // Step 2: native lua form of tostring on value
          lua_getglobal( L, "tostring" ); // <tostring> value key table
          lua_pushvalue( L, -2 ); // value <tostring> value key table

          if( lua_pcall( L, 1, 1, 0 ) ) { // error value key table
            Log::getInstance().error( "LuaKit::Serializer::printTableContents", lua_tostring( L, -1 ) );
            lua_pop( L, 4 ); // EMPTY
            return;
          } // "string representation" value key table

          std::string value( lua_tostring( L, -1 ) );

          Log::getInstance().debug( "LuaKit::Serializer::printTableContents", key + ": " + value );

          lua_pop( L, 2 ); // key table
        } // table


        lua_pop( L, 1 ); // EMPTY
      }
    }
  }
}
