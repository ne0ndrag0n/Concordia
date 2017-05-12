#ifndef INPUT_MANAGER
#define INPUT_MANAGER

#include <map>
#include <functional>
#include <memory>
#include <SFGUI/Container.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>

namespace BlueBear {
  class EventManager;

  namespace Graphics {
    namespace Input {

      class InputManager {
        std::shared_ptr< EventManager > eventManager;
        std::map< sf::Keyboard::Key, std::function< void() > > keyEvents;
        bool eatKeyEvents = false;
        bool eatMouseEvents = false;

        void removeSFGUIFocus();

      public:
        InputManager( std::shared_ptr< EventManager > eventManager );
        ~InputManager();
        void listen( sf::Keyboard::Key key, std::function< void() > callback );
        void handleEvent( sf::Event& event );
      };

    }
  }
}


#endif
