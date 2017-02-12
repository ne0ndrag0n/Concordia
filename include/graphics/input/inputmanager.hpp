#ifndef INPUT_MANAGER
#define INPUT_MANAGER

#include <map>
#include <functional>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>

namespace BlueBear {
  class EventManager;

  namespace Graphics {
    namespace Input {

      class InputManager {
        EventManager& eventManager;
        std::map< sf::Keyboard::Key, std::function< void() > > keyEvents;
        bool enableKeyEvents = true;

      public:
        InputManager( EventManager& eventManager );
        ~InputManager();
        void listen( sf::Keyboard::Key key, std::function< void() > callback );
        void handleEvent( sf::Event& event );
      };

    }
  }
}


#endif
