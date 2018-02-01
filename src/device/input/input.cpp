#include "device/input/input.hpp"
#include "graphics/gui/sfgroot.hpp"
#include "tools/ctvalidators.hpp"
#include "tools/utility.hpp"
#include "eventmanager.hpp"
#include "log.hpp"
#include "scripting/luastate.hpp"
#include "state/state.hpp"
#include <algorithm>
#include <string>

namespace BlueBear {
  namespace Device {
    namespace Input {

      Input::Input() {
        eventManager.SFGUI_EAT_EVENT.listen( SFGUIEatEvent::Event::EAT_KEYBOARD_EVENT, [ & ]() {
          eatKeyEvents = true;
        } );

        eventManager.SFGUI_EAT_EVENT.listen( SFGUIEatEvent::Event::EAT_MOUSE_EVENT, [ & ]() {
          eatMouseEvents = true;
        } );

        eventManager.LUA_STATE_READY.listen( this, std::bind( &Input::submitLuaContributions, this, std::placeholders::_1 ) );
      }

      void Input::submitLuaContributions( sol::state& lua ) {
        sol::table event = lua[ "bluebear" ][ "event" ];
        event.set_function( "register_key", &Input::registerScriptKey, this );
        event.set_function( "unregister_key", &Input::unregisterScriptKey, this );
      }

      Input::~Input() {
        eventManager.SFGUI_EAT_EVENT.stopListening( SFGUIEatEvent::Event::EAT_KEYBOARD_EVENT );
        eventManager.LUA_STATE_READY.stopListening( this );
      }

      /**
       * Whatta hack
       */
      void Input::removeSFGUIFocus() {
        Graphics::GUI::RootContainer::Create()->GrabFocus();
      }

      void Input::listen( sf::Keyboard::Key key, std::function< void() > callback ) {
        keyEvents[ key ] = callback;
      }

      void Input::handleEvent( sf::Event& event ) {
        switch( event.type ) {
          case sf::Event::KeyPressed:
            {
              if( eatKeyEvents == false ) {
                auto it = keyEvents.find( event.key.code );
                if( it != keyEvents.end() ) {
                  it->second();
                } else {
                  // Lua key events come second-fiddle to key events
                  auto it2 = luaKeyEvents.find( event.key.code );
                  if( it2 != luaKeyEvents.end() ) {
                    fireOff( it2->second );
                  }
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

      void Input::fireOff( std::vector< sol::function >& refs ) {
        for( sol::function copy : refs ) {
          if( copy.valid() ) {
            eventManager.UI_ACTION_EVENT.trigger( copy );
          }
        }
      }

      /**
       * grueling
       */
      sf::Keyboard::Key Input::stringToKey( const std::string& key ) {
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
          case Tools::Utility::hash( "0" ):
            return sf::Keyboard::Num0;
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

          case Tools::Utility::hash( "ret" ):
            return sf::Keyboard::Return;

          default:
            return sf::Keyboard::Unknown;
        }
      }

      std::string Input::keyToString( sf::Keyboard::Key key ) {
        switch( key ) {
          case sf::Keyboard::Q:
            return "q";
          case sf::Keyboard::W:
            return "w";
          case sf::Keyboard::E:
            return "e";
          case sf::Keyboard::R:
            return "r";
          case sf::Keyboard::T:
            return "t";
          case sf::Keyboard::Y:
            return "y";
          case sf::Keyboard::U:
            return "u";
          case sf::Keyboard::I:
            return "i";
          case sf::Keyboard::O:
            return "o";
          case sf::Keyboard::P:
            return "p";
          case sf::Keyboard::A:
            return "a";
          case sf::Keyboard::S:
            return "s";
          case sf::Keyboard::D:
            return "d";
          case sf::Keyboard::F:
            return "f";
          case sf::Keyboard::G:
            return "g";
          case sf::Keyboard::H:
            return "h";
          case sf::Keyboard::J:
            return "j";
          case sf::Keyboard::K:
            return "k";
          case sf::Keyboard::L:
            return "l";
          case sf::Keyboard::Z:
            return "z";
          case sf::Keyboard::X:
            return "x";
          case sf::Keyboard::C:
            return "c";
          case sf::Keyboard::V:
            return "v";
          case sf::Keyboard::B:
            return "b";
          case sf::Keyboard::N:
            return "n";
          case sf::Keyboard::M:
            return "m";

          case sf::Keyboard::Tilde:
            return "~";
          case sf::Keyboard::Num1:
            return "1";
          case sf::Keyboard::Num2:
            return "2";
          case sf::Keyboard::Num3:
            return "3";
          case sf::Keyboard::Num4:
            return "4";
          case sf::Keyboard::Num5:
            return "5";
          case sf::Keyboard::Num6:
            return "6";
          case sf::Keyboard::Num7:
            return "7";
          case sf::Keyboard::Num8:
            return "8";
          case sf::Keyboard::Num9:
            return "9";
          case sf::Keyboard::Num0:
            return "0";
          case sf::Keyboard::Dash:
            return "dash";
          case sf::Keyboard::Equal:
            return "equal";
          case sf::Keyboard::BackSpace:
            return "bksp";

          case sf::Keyboard::Escape:
            return "esc";
          case sf::Keyboard::F1:
            return "f1";
          case sf::Keyboard::F2:
            return "f2";
          case sf::Keyboard::F3:
            return "f3";
          case sf::Keyboard::F4:
            return "f4";
          case sf::Keyboard::F5:
            return "f5";
          case sf::Keyboard::F6:
            return "f6";
          case sf::Keyboard::F7:
            return "f7";
          case sf::Keyboard::F8:
            return "f8";
          case sf::Keyboard::F9:
            return "f9";
          case sf::Keyboard::F10:
            return "f10";
          case sf::Keyboard::F11:
            return "f11";
          case sf::Keyboard::F12:
            return "f12";


          case sf::Keyboard::Tab:
            return "tab";
          case sf::Keyboard::LBracket:
            return "[";
          case sf::Keyboard::RBracket:
            return "]";
          case sf::Keyboard::BackSlash:
            return "\\";
          case sf::Keyboard::SemiColon:
            return ";";
          case sf::Keyboard::Quote:
            return "'";
          case sf::Keyboard::Comma:
            return ",";
          case sf::Keyboard::Period:
            return ".";
          case sf::Keyboard::Slash:
            return "/";

          case sf::Keyboard::LControl:
            return "lctrl";
          case sf::Keyboard::LSystem:
            return "lsys";
          case sf::Keyboard::LAlt:
            return "lalt";

          case sf::Keyboard::RControl:
            return "rctrl";
          case sf::Keyboard::Menu:
            return "menu";
          case sf::Keyboard::RSystem:
            return "rsys";
          case sf::Keyboard::RAlt:
            return "ralt";

          case sf::Keyboard::LShift:
            return "lshift";
          case sf::Keyboard::RShift:
            return "rshift";

          case sf::Keyboard::Insert:
            return "ins";
          case sf::Keyboard::Home:
            return "home";
          case sf::Keyboard::PageUp:
            return "pgup";
          case sf::Keyboard::Delete:
            return "del";
          case sf::Keyboard::End:
            return "end";
          case sf::Keyboard::PageDown:
            return "pgdn";

          case sf::Keyboard::Up:
            return "up";
          case sf::Keyboard::Down:
            return "down";
          case sf::Keyboard::Left:
            return "left";
          case sf::Keyboard::Right:
            return "right";

          case sf::Keyboard::Return:
            return "ret";

          default:
            return "<unk>";
        }
      }

      unsigned int Input::insertNearest( sf::Keyboard::Key key, sol::function& function ) {
        auto& collection = luaKeyEvents[ key ];

        for( int i = 0; i != collection.size(); i++ ) {
          if( !collection[ i ].valid() ) {
            collection[ i ] = function;
            return i;
          }
        }

        collection.push_back( function );
        return collection.size() - 1;
      }

      sol::variadic_results Input::registerScriptKey( sol::this_state L, const std::string& key, sol::function callback ) {
        sol::variadic_results result;

        if( callback.valid() ) {
          sf::Keyboard::Key sfKey = stringToKey( key );
          auto it = keyEvents.find( sfKey );
          if( it == keyEvents.end() ) {
            result.push_back( { L, sol::in_place, insertNearest( sfKey, callback ) } );
          } else {
            Log::getInstance().warn( "Input::lua_registerScriptKey", "This key is reserved by the engine and cannot be registered for an event." );
          }
        } else {
          Log::getInstance().error( "Input::registerScriptKey", "Invalid function provided to bluebear.event.register_key" );
        }

        return result;
      }

      void Input::unregisterScriptKey( const std::string& key, int id ) {
        sf::Keyboard::Key sfKey = stringToKey( key );
        if( luaKeyEvents.find( sfKey ) != luaKeyEvents.end() ) {
          std::vector< sol::function >& vector = luaKeyEvents[ sfKey ];
          if( id < vector.size() ) {
            vector[ id ] = sol::function{};
          }
        }
      }

    }
  }
}
