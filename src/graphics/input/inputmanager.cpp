#include "graphics/input/inputmanager.hpp"
#include "eventmanager.hpp"
#include "log.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace Input {

      InputManager::InputManager( EventManager& eventManager ) : eventManager( eventManager ) {
        eventManager.SFGUI_EAT_EVENT.listen( SFGUIEatEvent::Event::EAT_KEYBOARD_EVENT, [ & ]() {
          eatKeyEvents = true;
        } );

        eventManager.SFGUI_EAT_EVENT.listen( SFGUIEatEvent::Event::EAT_MOUSE_EVENT, [ & ]() {
          //Log::getInstance().debug( "Assert", "Staging to eat mouse event..." );
          eatMouseEvents = true;
        } );
      }

      InputManager::~InputManager() {
        eventManager.SFGUI_EAT_EVENT.stopListening( SFGUIEatEvent::Event::EAT_KEYBOARD_EVENT );
      }

      void InputManager::removeSFGUIFocus() {
        // FIXME the focus on the rootcontainer goofs things up a bit
        //this->rootContainer->GrabFocus();
      }

      void InputManager::listen( sf::Keyboard::Key key, std::function< void() > callback ) {
        keyEvents[ key ] = callback;
      }

      void InputManager::handleEvent( sf::Event& event ) {
        switch( event.type ) {
          case sf::Event::KeyPressed:
            {
              if( eatKeyEvents == false ) {
                auto it = keyEvents.find( event.key.code );
                if( it != keyEvents.end() ) {
                  it->second();
                }
              } else {
                // Only eat the event for this tick
                eatKeyEvents = false;
              }
            }
            break;
          case sf::Event::MouseButtonPressed:
            {
              if( eatMouseEvents == false ) {
                //Log::getInstance().debug( "Assert", "GUI didn't eat this event" );
                removeSFGUIFocus();
                // TODO mouse shit
              } else {
                //Log::getInstance().debug( "Assert", "GUI ate this event" );
                eatMouseEvents = false;
              }
            }
          default:
            break;
        }
      }

      void InputManager::setRootContainer( std::shared_ptr< sfg::Container > rootContainer ) {
        this->rootContainer = rootContainer;
      }

    }
  }
}
