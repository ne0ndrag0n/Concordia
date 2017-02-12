#include "eventmanager.hpp"

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

  void SFGUISignalEvent::listen( SFGUISignalEvent::Event event, std::function< void() > callback ) {
    listeners[ event ] = callback;
  }

  void SFGUISignalEvent::stopListening( SFGUISignalEvent::Event event ) {
    listeners.erase( event );
  }

  void SFGUISignalEvent::trigger( SFGUISignalEvent::Event event ) {
    listeners.at( event )();
  }
}
