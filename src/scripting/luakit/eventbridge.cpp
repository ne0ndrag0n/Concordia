#include "scripting/luakit/eventbridge.hpp"
#include "scripting/coreengine.hpp"
#include "eventmanager.hpp"

namespace BlueBear::Scripting::LuaKit {

  EventBridge::EventBridge( CoreEngine& coreEngine ) : coreEngine( coreEngine ) {
    eventManager.MESSAGE_LOGGED.listen( this, std::bind( &EventBridge::fireEvents, this, "message-logged", std::placeholders::_1 ) );
  }

  EventBridge::~EventBridge() {
    eventManager.MESSAGE_LOGGED.stopListening( this );
  }

  void EventBridge::submitLuaContributions( sol::table event ) {
    event.set_function( "register_system_event", &EventBridge::registerEvent, this );
    event.set_function( "unregister_system_event", &EventBridge::unregisterEvent, this );
  }

  int EventBridge::registerEvent( const std::string& key, sol::function f ) {
    auto it = registered.find( key );

    if( it == registered.end() ) {
      registered[ key ] = Containers::ReusableObjectVector< sol::function >{};
      it = registered.find( key );
    }

    return it->second.insert( f );
  }

  void EventBridge::unregisterEvent( const std::string& key, int index ) {
    auto it = registered.find( key );

    if( it != registered.end() ) {
      it->second.remove( index );
    }
  }

  void EventBridge::fireEvents( const std::string& key, const std::string& arg1 ) {
    auto it = registered.find( key );

    if( it != registered.end() ) {
      it->second.each( [ & ]( sol::function& f ) {
        coreEngine.setTimeout( 0, [ &f, arg1 ]() {
          f( arg1 );
        } );
      } );
    }
  }

}
