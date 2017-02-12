#include "graphics/input/inputmanager.hpp"
#include "eventmanager.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace Input {

      InputManager::InputManager( EventManager& eventManager ) : eventManager( eventManager ) {
        eventManager.SFGUI_SIGNAL_EVENT.listen( SFGUISignalEvent::Event::ENABLE_KEYBOARD_EVENTS, [ & ]() {
          enableKeyEvents = true;
        } );

        eventManager.SFGUI_SIGNAL_EVENT.listen( SFGUISignalEvent::Event::DISABLE_KEYBOARD_EVENTS, [ & ]() {
          enableKeyEvents = false;
        } );
      }

      InputManager::~InputManager() {
        eventManager.SFGUI_SIGNAL_EVENT.stopListening( SFGUISignalEvent::Event::ENABLE_KEYBOARD_EVENTS );
        eventManager.SFGUI_SIGNAL_EVENT.stopListening( SFGUISignalEvent::Event::DISABLE_KEYBOARD_EVENTS );
      }

      void InputManager::listen( sf::Keyboard::Key key, std::function< void() > callback ) {
        keyEvents[ key ] = callback;
      }

      void InputManager::handleEvent( sf::Event& event ) {
        switch( event.type ) {
          case sf::Event::KeyPressed:
            {
              if( enableKeyEvents == true ) {
                auto it = keyEvents.find( event.key.code );
                if( it != keyEvents.end() ) {
                  it->second();
                }
              }
            }
            break;
          default:
            break;
        }
      }


    }
  }
}
