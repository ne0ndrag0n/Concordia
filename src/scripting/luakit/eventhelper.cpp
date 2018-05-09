#include "scripting/luakit/eventhelper.hpp"
#include "scripting/coreengine.hpp"
#include "device/input/input.hpp"
#include "eventmanager.hpp"
#include <SFML/Window/Event.hpp>
#include <functional>

#include "log.hpp"

namespace BlueBear::Scripting::LuaKit {

  EventHelper::EventHelper( CoreEngine& engine ) : engine( engine ) {
    eventManager.LUA_STATE_READY.listen( this, std::bind( &EventHelper::submitLuaContributions, this, std::placeholders::_1 ) );
  }

  EventHelper::~EventHelper() {
    eventManager.LUA_STATE_READY.stopListening( this );
  }

  void EventHelper::submitLuaContributions( sol::state& lua ) {
    if( lua[ "bluebear" ][ "event" ] == sol::nil ) {
      lua[ "bluebear" ][ "event" ] = lua.create_table();
    }

    sol::table event = lua[ "bluebear" ][ "event" ];
    event.set_function( "register_key", &EventHelper::registerKey, this );
    event.set_function( "unregister_key", &EventHelper::unregisterKey, this );
  }

  int EventHelper::registerKey( const std::string& key, sol::function f ) {
    return keyEvents[ key ].insert( f );
  }

  void EventHelper::unregisterKey( const std::string& key, int handle ) {
    keyEvents[ key ].remove( handle );
  }

  void EventHelper::onKeyDown( Device::Input::Metadata event ) {
    auto it = keyEvents.find( event.keyPressed );
    if( it != keyEvents.end() ) {
      it->second.each( [ & ]( sol::function& function ) {
        engine.setTimeout( 0, function );
      } );
    }
  }

  void EventHelper::connectInputDevice( Device::Input::Input& inputDevice ) {
    inputDevice.registerInputEvent( sf::Event::KeyPressed, std::bind( &EventHelper::onKeyDown, this, std::placeholders::_1 ) );
  }

}
