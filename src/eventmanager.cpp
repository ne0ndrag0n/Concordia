#include "eventmanager.hpp"
#include "log.hpp"

namespace BlueBear {

  void UIActionEvent::listen( void* key, std::function< void( LuaReference ) > callback ) {
    listeners[ key ] = callback;
  }

  void UIActionEvent::stopListening( void* key ) {
    listeners.erase( key );
  }

  void UIActionEvent::trigger( LuaReference param ) {
    for( auto& nestedPair : listeners ) {
      nestedPair.second( param );
    }
  }

  void SFGUIEatEvent::listen( SFGUIEatEvent::Event event, std::function< void() > callback ) {
    listeners[ event ] = callback;
  }

  void SFGUIEatEvent::stopListening( SFGUIEatEvent::Event event ) {
    listeners.erase( event );
  }

  void SFGUIEatEvent::trigger( SFGUIEatEvent::Event event ) {
    listeners.at( event )();
  }
}
