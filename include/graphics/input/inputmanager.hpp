#ifndef INPUT_MANAGER
#define INPUT_MANAGER

#include "bbtypes.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
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
        lua_State* L;
        std::map< sf::Keyboard::Key, std::function< void() > > keyEvents;
        std::map< sf::Keyboard::Key, std::vector< LuaReference > > luaKeyEvents;

        bool eatKeyEvents = false;
        bool eatMouseEvents = false;

        void removeSFGUIFocus();
        static sf::Keyboard::Key stringToKey( const std::string& key );

        unsigned int insertNearest( std::vector< LuaReference >& vector, LuaReference value );
        void fireOff( std::vector< LuaReference >& refs );

      public:
        InputManager( lua_State* L );
        ~InputManager();
        void listen( sf::Keyboard::Key key, std::function< void() > callback );
        void handleEvent( sf::Event& event );
        static int lua_registerScriptKey( lua_State* L );
        static int lua_unregisterScriptKey( lua_State* L );

      };

    }
  }
}


#endif
