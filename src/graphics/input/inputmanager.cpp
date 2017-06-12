#include "graphics/input/inputmanager.hpp"
#include "graphics/display.hpp"
#include "graphics/gui/sfgroot.hpp"
#include "tools/ctvalidators.hpp"
#include "tools/utility.hpp"
#include "eventmanager.hpp"
#include "log.hpp"
#include <algorithm>
#include <string>

namespace BlueBear {
  namespace Graphics {
    namespace Input {

      InputManager::InputManager() {
        eventManager.SFGUI_EAT_EVENT.listen( SFGUIEatEvent::Event::EAT_KEYBOARD_EVENT, [ & ]() {
          eatKeyEvents = true;
        } );

        eventManager.SFGUI_EAT_EVENT.listen( SFGUIEatEvent::Event::EAT_MOUSE_EVENT, [ & ]() {
          eatMouseEvents = true;
        } );
      }

      InputManager::~InputManager() {
        eventManager.SFGUI_EAT_EVENT.stopListening( SFGUIEatEvent::Event::EAT_KEYBOARD_EVENT );
      }

      /**
       * Whatta hack
       */
      void InputManager::removeSFGUIFocus() {
        GUI::RootContainer::Create()->GrabFocus();
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
          case sf::Event::MouseButtonReleased:
            {
              if( eatMouseEvents == false ) {
                removeSFGUIFocus();
                // TODO mouse shit
              } else {
                eatMouseEvents = false;
              }
            }
          default:
            break;
        }
      }

      /**
       * grueling
       */
      sf::Keyboard::Key InputManager::stringToKey( const std::string& key ) {
        std::string copy = key;

        if( copy.length() == 0 ) {
          return sf::Keyboard::Unknown;
        }

        // fuckin' language should have a lowercase method built into it. don't use unicode here
        std::transform( copy.begin(), copy.end(), copy.begin(), ::tolower );

        switch( Tools::Utility::hash( copy.c_str() ) ) {
          case Tools::Utility::hash( "q" ):
            return sf::Keyboard::Q;
          case Tools::Utility::hash( "w" ):
            return sf::Keyboard::W;
          case Tools::Utility::hash( "e" ):
            return sf::Keyboard::E;
          case Tools::Utility::hash( "r" ):
            return sf::Keyboard::R;
          case Tools::Utility::hash( "t" ):
            return sf::Keyboard::T;
          case Tools::Utility::hash( "y" ):
            return sf::Keyboard::Y;
          case Tools::Utility::hash( "u" ):
            return sf::Keyboard::U;
          case Tools::Utility::hash( "i" ):
            return sf::Keyboard::I;
          case Tools::Utility::hash( "o" ):
            return sf::Keyboard::O;
          case Tools::Utility::hash( "p" ):
            return sf::Keyboard::P;
          case Tools::Utility::hash( "a" ):
            return sf::Keyboard::A;
          case Tools::Utility::hash( "s" ):
            return sf::Keyboard::S;
          case Tools::Utility::hash( "d" ):
            return sf::Keyboard::D;
          case Tools::Utility::hash( "f" ):
            return sf::Keyboard::F;
          case Tools::Utility::hash( "g" ):
            return sf::Keyboard::G;
          case Tools::Utility::hash( "h" ):
            return sf::Keyboard::H;
          case Tools::Utility::hash( "j" ):
            return sf::Keyboard::J;
          case Tools::Utility::hash( "k" ):
            return sf::Keyboard::K;
          case Tools::Utility::hash( "l" ):
            return sf::Keyboard::L;
          case Tools::Utility::hash( "z" ):
            return sf::Keyboard::Z;
          case Tools::Utility::hash( "x" ):
            return sf::Keyboard::X;
          case Tools::Utility::hash( "c" ):
            return sf::Keyboard::C;
          case Tools::Utility::hash( "v" ):
            return sf::Keyboard::V;
          case Tools::Utility::hash( "b" ):
            return sf::Keyboard::B;
          case Tools::Utility::hash( "n" ):
            return sf::Keyboard::N;
          case Tools::Utility::hash( "m" ):
            return sf::Keyboard::M;

          case Tools::Utility::hash( "~" ):
            return sf::Keyboard::Tilde;
          case Tools::Utility::hash( "1" ):
            return sf::Keyboard::Num1;
          case Tools::Utility::hash( "2" ):
            return sf::Keyboard::Num2;
          case Tools::Utility::hash( "3" ):
            return sf::Keyboard::Num3;
          case Tools::Utility::hash( "4" ):
            return sf::Keyboard::Num4;
          case Tools::Utility::hash( "5" ):
            return sf::Keyboard::Num5;
          case Tools::Utility::hash( "6" ):
            return sf::Keyboard::Num6;
          case Tools::Utility::hash( "7" ):
            return sf::Keyboard::Num7;
          case Tools::Utility::hash( "8" ):
            return sf::Keyboard::Num8;
          case Tools::Utility::hash( "9" ):
            return sf::Keyboard::Num9;
          case Tools::Utility::hash( "dash" ):
            return sf::Keyboard::Dash;
          case Tools::Utility::hash( "equal" ):
            return sf::Keyboard::Equal;
          case Tools::Utility::hash( "bksp" ):
            return sf::Keyboard::BackSpace;


          case Tools::Utility::hash( "esc" ):
            return sf::Keyboard::Escape;
          case Tools::Utility::hash( "f1" ):
            return sf::Keyboard::F1;
          case Tools::Utility::hash( "f2" ):
            return sf::Keyboard::F2;
          case Tools::Utility::hash( "f3" ):
            return sf::Keyboard::F3;
          case Tools::Utility::hash( "f4" ):
            return sf::Keyboard::F4;
          case Tools::Utility::hash( "f5" ):
            return sf::Keyboard::F5;
          case Tools::Utility::hash( "f6" ):
            return sf::Keyboard::F6;
          case Tools::Utility::hash( "f7" ):
            return sf::Keyboard::F7;
          case Tools::Utility::hash( "f8" ):
            return sf::Keyboard::F8;
          case Tools::Utility::hash( "f9" ):
            return sf::Keyboard::F9;
          case Tools::Utility::hash( "f10" ):
            return sf::Keyboard::F10;
          case Tools::Utility::hash( "f11" ):
            return sf::Keyboard::F11;
          case Tools::Utility::hash( "f12" ):
            return sf::Keyboard::F12;


          case Tools::Utility::hash( "tab" ):
            return sf::Keyboard::Tab;
          case Tools::Utility::hash( "[" ):
            return sf::Keyboard::LBracket;
          case Tools::Utility::hash( "]" ):
            return sf::Keyboard::RBracket;
          case Tools::Utility::hash( "\\" ):
            return sf::Keyboard::BackSlash;
          case Tools::Utility::hash( ";" ):
            return sf::Keyboard::SemiColon;
          case Tools::Utility::hash( "'" ):
            return sf::Keyboard::Quote;
          case Tools::Utility::hash( "," ):
            return sf::Keyboard::Comma;
          case Tools::Utility::hash( "." ):
            return sf::Keyboard::Period;
          case Tools::Utility::hash( "/" ):
            return sf::Keyboard::Slash;


          case Tools::Utility::hash( "lctrl" ):
            return sf::Keyboard::LControl;
          case Tools::Utility::hash( "lsys" ):
            return sf::Keyboard::LSystem;
          case Tools::Utility::hash( "lalt" ):
            return sf::Keyboard::LAlt;


          case Tools::Utility::hash( "rctrl" ):
            return sf::Keyboard::RControl;
          case Tools::Utility::hash( "menu" ):
            return sf::Keyboard::Menu;
          case Tools::Utility::hash( "rsys" ):
            return sf::Keyboard::RSystem;
          case Tools::Utility::hash( "ralt" ):
            return sf::Keyboard::RAlt;


          case Tools::Utility::hash( "lshift" ):
            return sf::Keyboard::LShift;
          case Tools::Utility::hash( "rshift" ):
            return sf::Keyboard::RShift;

          case Tools::Utility::hash( "ins" ):
            return sf::Keyboard::Insert;
          case Tools::Utility::hash( "home" ):
            return sf::Keyboard::Home;
          case Tools::Utility::hash( "pgup" ):
            return sf::Keyboard::PageUp;
          case Tools::Utility::hash( "del" ):
            return sf::Keyboard::Delete;
          case Tools::Utility::hash( "end" ):
            return sf::Keyboard::End;
          case Tools::Utility::hash( "pgdn" ):
            return sf::Keyboard::PageDown;

          case Tools::Utility::hash( "up" ):
            return sf::Keyboard::Up;
          case Tools::Utility::hash( "down" ):
            return sf::Keyboard::Down;
          case Tools::Utility::hash( "left" ):
            return sf::Keyboard::Left;
          case Tools::Utility::hash( "right" ):
            return sf::Keyboard::Right;

          default:
            return sf::Keyboard::Unknown;
        }
      }

      int InputManager::lua_registerScriptKey( lua_State* L ) {
        VERIFY_FUNCTION_N( "InputManager::lua_registerScriptKey", "register_key", 1 );
        VERIFY_STRING_N( "InputManager::lua_registerScriptKey", "register_key", 2 );

        Display::MainGameState* state = ( Display::MainGameState* )lua_touserdata( L, lua_upvalueindex( 1 ) );
        InputManager& self = state->getInputManager();

        sf::Keyboard::Key sfKey = stringToKey( lua_tostring( L, -1 ) );
        auto it = self.keyEvents.find( sfKey );
        if( it == self.keyEvents.end() ) {
          std::vector< LuaReference >& refTable = self.luaKeyEvents[ sfKey ];
          auto ref = luaL_ref( L, -1 );
          refTable.push_back( ref );

          lua_pushnumber( L, ref ); // 42
          return 1;
        } else {
          Log::getInstance().warn( "InputManager::lua_registerScriptKey", "This key is reserved by the engine and cannot be registered for an event." );
        }

        return 0;
      }

      int InputManager::lua_unregisterScriptKey( lua_State* L ) {
        Display::MainGameState* state = ( Display::MainGameState* )lua_touserdata( L, lua_upvalueindex( 1 ) );
        InputManager& self = state->getInputManager();

        return 0;
      }

    }
  }
}
