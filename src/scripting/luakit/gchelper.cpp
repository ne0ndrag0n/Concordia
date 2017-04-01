#include "scripting/luakit/gchelper.hpp"


namespace BlueBear {
  namespace Scripting {
    namespace LuaKit {

      /**
       * Tracks an element on the GcHelper, signing a guarantee that the object will not be garbage-collected as long as the GcHelper holds onto it.
       *
       * STACK ARGS: object
       * Stack is unmodified after call
       */
      void GcHelper::track( lua_State* L, const std::string& tag ) {
        lua_pushstring( L, "bluebear_gc_helper" ); // "bluebear_gc_helper" object
        lua_gettable( L, LUA_REGISTRYINDEX ); // table object

        lua_pushstring( L, tag.c_str() ); // "key" table object
        lua_pushvalue( L, -3 ); // object "key" table object
        lua_settable( L, -3 ); // table object

        lua_pop( L, 1 ); // object
      }

      /**
       * Free an element for potential garbage collection. Note: this only releases GcHelper's contract that the object will never be GC'd; it does not mean it is available to be GC'd
       * if the object is held elsewhere.
       *
       * STACK ARGS: none
       * Stack is unmodified after call
       */
       void GcHelper::untrack( lua_State* L, const std::string& tag ) {
        lua_pushstring( L, "bluebear_gc_helper" ); // "bluebear_gc_helper"
        lua_gettable( L, LUA_REGISTRYINDEX ); // table

        lua_pushstring( L, tag.c_str() ); // "key" table
        lua_pushnil( L ); // nil "key" table
        lua_settable( L, -3 ); // table

        lua_pop( L, 1 ); // EMPTY
      }

      /**
       * Initialize the Concordia GcHelper. This clears out any existing values and makes them immediately available for garbage collection; use with care!
       *
       * STACK ARGS: none
       * Stack is unmodified after call
       */
      void GcHelper::initialize( lua_State* L ) {
        lua_pushstring( L, "bluebear_gc_helper" ); // "bluebear_gc_helper"
        lua_newtable( L ); // table "bluebear_gc_helper"
        lua_settable( L, LUA_REGISTRYINDEX ); // EMPTY
      }
    }
  }
}
