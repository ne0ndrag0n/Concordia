#include "scripting/coreengine.hpp"
#include "scripting/luakit/modpackloader.hpp"
#include "tools/utility.hpp"
#include "configmanager.hpp"
#include "eventmanager.hpp"
#include "log.hpp"
#include <cmath>

namespace BlueBear::Scripting {

  CoreEngine::CoreEngine( State::State& state ) : State::Substate( state ) {
    luaL_openlibs( lua.lua_state() );
    setupCoreEnvironment();
  }

  void CoreEngine::setupCoreEnvironment() {
    lua[ "bluebear" ] = lua.create_table();

    sol::table engine = lua.create_table();
    engine.set_function( "require_modpack", []() {} );
    engine.set_function( "queue_callback", &CoreEngine::setTimeout, this );

    sol::table util = lua.create_table();
    util.set_function( "bind", &CoreEngine::bind, this );
    util.set_function( "seconds_to_ticks", &CoreEngine::secondsToTicks );

    lua.set_function( "print", sol::overload(
      [ & ]( const std::string& tag, const std::string& message ) {
        Log::getInstance().debug( tag, message );
      },
      [ & ]( const std::string& message ) {
        Log::getInstance().debug( "<script>", message );
      }
    ) );

    lua[ "bluebear" ][ "engine" ] = engine;
    lua[ "bluebear" ][ "util" ] = util;
  }

  sol::function CoreEngine::bind( sol::function f, sol::variadic_args args ) {
    sol::table temp = lua.create_table();
    temp.set_function( "__closure", [ f, unpacked = std::vector< sol::object >( args.begin(), args.end() ) ]( sol::variadic_args newargs ) {
      std::vector< sol::object > newunpacked( newargs.begin(), newargs.end() );

      return f( sol::as_args( Tools::Utility::concatArrays( unpacked, newunpacked ) ) );
    } );

    return temp[ "__closure" ];
  }

  int CoreEngine::setTimeout( double interval, sol::function f ) {
    return queuedCallbacks.insert( { std::round( interval ), f } );
  }

  double CoreEngine::secondsToTicks( double seconds ) {
    return seconds * ConfigManager::getInstance().getIntValue( "fps_overview" );
  }

  void CoreEngine::loadModpacks() {
    {
      LuaKit::ModpackLoader sys( lua, SYSTEM_MODPACK_DIRECTORY );
      sys.load();
    }

    {
      LuaKit::ModpackLoader user( lua, USER_MODPACK_DIRECTORY );
      user.load();
    }
  }

  void CoreEngine::broadcastReadyEvent() {
    eventManager.LUA_STATE_READY.trigger( lua );
  }

  bool CoreEngine::update() {
    std::vector< int > removalIndices;
    int i = 0;

    queuedCallbacks.each( [ & ]( std::optional< std::pair< int, std::variant< sol::function, std::function< void() > > > >& optional ) {
      if( optional ) {
        std::pair< int, std::variant< sol::function, std::function< void() > > >& callback = *optional;

        if( callback.first == 0 ) {
          std::visit( overloaded {
            [ & ]( sol::function function ) {
              auto result = function();
              if( !result.valid() ) {
                sol::error error = result;
                Log::getInstance().error( "CoreEngine::update", "Exception thrown: " + std::string( error.what() ) );
              }
            },
            []( std::function< void() > function ) {
              try {
                function();
              } catch( std::exception& error ) {
                Log::getInstance().error( "CoreEngine::update", "Exception thrown: " + std::string( error.what() ) );
              }
            }
          }, callback.second );

          removalIndices.push_back( i );
        } else {
          callback.first = std::max( 0, callback.first - 1 );
        }
      }

      i++;
    } );

    for( int removal : removalIndices ) {
      queuedCallbacks.remove( removal );
    }
  }

}
