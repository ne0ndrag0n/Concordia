#ifndef BB_DEVICE_INPUT
#define BB_DEVICE_INPUT

#include "bbtypes.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <unordered_map>
#include <vector>
#include <functional>
#include <memory>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>
#include <sol.hpp>
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
      };

      class Input {
      public:
        class KeyGroup {
          std::unordered_map< std::string, std::function< void() > > keyEvents;
          std::unordered_map< std::string, std::vector< sol::function > > luaKeyEvents;

          unsigned int insertNearest( const std::string& key, sol::function& function );
          void fireOff( std::vector< sol::function >& refs );
          void submitLuaContributions( sol::state& lua );

        public:
          KeyGroup();
          ~KeyGroup();

          void registerSystemKey( const std::string& key, std::function< void() > callback );
          sol::variadic_results registerScriptKey( sol::this_state L, const std::string& key, sol::function callback );

          void unregisterScriptKey( const std::string& key, int id );
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
