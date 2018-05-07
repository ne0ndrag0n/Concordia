#include "scripting/coreengine.hpp"
#include "scripting/luakit/modpackloader.hpp"
#include "tools/utility.hpp"
#include "eventmanager.hpp"
#include "log.hpp"

namespace BlueBear::Scripting {

  CoreEngine::CoreEngine( State::State& state ) : State::Substate( state ) {
    luaL_openlibs( lua.lua_state() );
    setupCoreEnvironment();

    {
      LuaKit::ModpackLoader sys( lua, SYSTEM_MODPACK_DIRECTORY );
      sys.load();
    }

    {
      LuaKit::ModpackLoader user( lua, USER_MODPACK_DIRECTORY );
      user.load();
    }

    eventManager.LUA_STATE_READY.trigger( lua );
  }

  void CoreEngine::setupCoreEnvironment() {
    lua[ "bluebear" ] = sol::table{};

    sol::table engine;
    engine[ "require_modpack" ] = []() {};
    engine.set_function( "queue_callback", &CoreEngine::setTimeout, this );

    sol::table util;
    util.set_function( "bind", &CoreEngine::bind );

    lua[ "bluebear" ][ "engine" ] = engine;
    lua[ "bluebear" ][ "util" ] = util;
  }

  sol::function CoreEngine::bind( sol::function f, sol::variadic_args args ) {
    sol::table temp;
    temp.set_function( "__closure", [ f, unpacked = std::vector< sol::object >( args.begin(), args.end() ) ]( sol::variadic_args newargs ) {
      std::vector< sol::object > newunpacked( newargs.begin(), newargs.end() );

      return f( sol::as_args( Tools::Utility::concatArrays( unpacked, newunpacked ) ) );
    } );

    return temp[ "__closure" ];
  }

  int CoreEngine::setTimeout( int interval, sol::function f ) {
    return queuedCallbacks.insert( { interval, f } );
  }

  bool CoreEngine::update() {
    queuedCallbacks.each( [ & ]( std::optional< std::pair< int,  std::variant< sol::function, std::function< void() > > > >& optional ) {
      if( optional ) {
        std::pair< int, std::variant< sol::function, std::function< void() > > >& callback = *optional;

        if( callback.first == 0 ) {
          std::visit( overloaded {
            [ & ]( sol::function function ) {
              // TODO: The traceback method
              sol::protected_function call( function );
              auto result = call();
              if( !result.valid() ) {
                sol::error error = result;
                Log::getInstance().error( "CoreEngine::update", "Exception thrown: " + std::string( error.what() ) );
              }
            },
            []( std::function< void() > function ) { function(); }
          }, callback.second );

          optional.reset();
        } else {
          callback.first = std::max( 0, callback.first - 1 );
        }
      }
    } );
  }

}
