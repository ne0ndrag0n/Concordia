#ifndef BB_DEVICE_INPUT
#define BB_DEVICE_INPUT

#include "bbtypes.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <map>
#include <functional>
#include <memory>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>
#include <sol.hpp>

namespace BlueBear {
  class Application;

  namespace Device {
    namespace Input {

      class Input {
        Application& application;
        std::map< sf::Keyboard::Key, std::function< void() > > keyEvents;
        std::map< sf::Keyboard::Key, std::vector< sol::function > > luaKeyEvents;
        bool eatKeyEvents = false;
        bool eatMouseEvents = false;

        void submitLuaContributions( sol::state& lua );
        void fireOff( std::vector< sol::function >& refs );
        unsigned int insertNearest( sf::Keyboard::Key key, sol::function& function );

        static sf::Keyboard::Key stringToKey( const std::string& key );
        static std::string keyToString( sf::Keyboard::Key key );

      public:
        Input( Application& application );
        ~Input();

        void reset();

        void listen( sf::Keyboard::Key key, std::function< void() > callback );
        void handleEvent( sf::Event& event );

        sol::variadic_results registerScriptKey( sol::this_state L, const std::string& key, sol::function callback );
        void unregisterScriptKey( const std::string& key, int id );

        void update();
      };

    }
  }
}

#endif
