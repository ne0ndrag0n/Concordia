#ifndef BB_DEVICE_INPUT
#define BB_DEVICE_INPUT

#include "bbtypes.hpp"
#include <unordered_map>
#include <vector>
#include <functional>
#include <memory>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>
#include <string>
#include <glm/glm.hpp>

namespace BlueBear {
  class Application;

  namespace Device {
    namespace Input {

      struct Metadata {
        std::string keyPressed;
        bool altModifier = false;
        bool ctrlModifier = false;
        bool shiftModifier = false;
        bool metaModifier = false;

        glm::ivec2 mouseLocation;
        bool leftMouse = false;
        bool middleMouse = false;
        bool rightMouse = false;

        std::function< void() > cancelAll;
      };

      class Input {
      public:
        class KeyGroup {
          std::unordered_map< std::string, std::function< void() > > keyEvents;

        public:
          void registerSystemKey( const std::string& key, std::function< void() > callback );
          void unregisterSystemKey( const std::string& key );

          void trigger( const std::string& key );
        };

      private:
        Application& application;
        std::unordered_map< sf::Event::EventType, std::vector< std::function< void( Metadata ) > > > events;
        bool eatKeyEvents = false;
        bool eatMouseEvents = false;
        void handleEvent( sf::Event& event );

      public:
        static sf::Keyboard::Key stringToKey( const std::string& key );
        static std::string keyToString( sf::Keyboard::Key key );
        static std::string getShifty( const std::string& key );

        Input( Application& application );

        unsigned int registerInputEvent( sf::Event::EventType type, std::function< void( Metadata ) > callback );
        void unregisterInputEvent( sf::Event::EventType type, int id );

        void reset();
        void update();
      };

    }
  }
}

#endif
