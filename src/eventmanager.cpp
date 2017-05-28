#include "eventmanager.hpp"
#include "log.hpp"

namespace BlueBear {

  EventManager eventManager;

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
