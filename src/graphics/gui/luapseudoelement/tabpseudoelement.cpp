#include "graphics/gui/luapseudoelement/tabpseudoelement.hpp"
#include "graphics/gui/luaelement.hpp"
#include "graphics/widgetbuilder.hpp"
#include "tools/ctvalidators.hpp"
#include "log.hpp"
#include <tinyxml2.h>

namespace BlueBear {
  namespace Graphics {
    namespace GUI {

      TabPseudoElement::TabPseudoElement( std::shared_ptr< sfg::Notebook > subject, unsigned int pageNumber, Display::MainGameState& displayState )
        : subject( subject ), pageNumber( pageNumber ), displayState( displayState ), stagedWidget( nullptr ) {}

      /**
       *
       * STACK ARGS: userdata
       * (Stack is unmodified after call)
       */
      void TabPseudoElement::setMetatable( lua_State* L ) {
        if( luaL_newmetatable( L, "bluebear_tab_pseudo_element" ) ) { // metatable userdata
          luaL_Reg tableFuncs[] = {
            { "add", TabPseudoElement::lua_add },
            { "remove", TabPseudoElement::lua_removeWidget },
            { "get_name", TabPseudoElement::lua_getName },
            { "find_pseudo", TabPseudoElement::lua_findElement },
            { "get_property", TabPseudoElement::lua_getProperty },
            { "set_property", TabPseudoElement::lua_setProperty },
            { "get_content", TabPseudoElement::lua_getContent },
            { "set_content", TabPseudoElement::lua_setContent },
            { "__gc", TabPseudoElement::lua_gc },
            { NULL, NULL }
          };

          luaL_setfuncs( L, tableFuncs, 0 );

          lua_pushvalue( L, -1 ); // metatable metatable userdata

          lua_setfield( L, -2, "__index" ); // metatable userdata
        }

        lua_setmetatable( L, -2 ); // userdata
      }

      /**
       * @static
       * Create and push a new unstaged userdata for a <tab> pseudoelement. This can be disconnected/connected to pages before they are staged.
       *
       * STACK ARGS: (none)
       * Returns: userdata, or none
       */
      int TabPseudoElement::lua_create( lua_State* L, Display::MainGameState& displayState, const std::string& xml ) {
        tinyxml2::XMLDocument document;
        document.Parse( xml.c_str() );

        if( document.ErrorID() ) {
          Log::getInstance().error( "TabPseudoElement::lua_create", "Could not parse XML string!" );
          return 0;
        }

        tinyxml2::XMLElement* element = document.RootElement();
        if( std::string( element->Name() ) == "tab" ) {
          tinyxml2::XMLElement* child = element->FirstChildElement();
          if( child != NULL ) {
            WidgetBuilder widgetBuilder( displayState.instance.eventManager, displayState.getImageCache() );
            std::shared_ptr< sfg::Widget > widget;

            try {
              widget = widgetBuilder.nodeToWidget( child );
            } catch( std::exception& e ) {
              Log::getInstance().error( "TabPseudoElement::lua_create", "Failed to add widget XML: " + std::string( e.what() ) );
              return 0;
            }

            TabPseudoElement** userData = ( TabPseudoElement** ) lua_newuserdata( L, sizeof( TabPseudoElement* ) ); // userdata
            *userData = new TabPseudoElement( nullptr, 0, displayState );
            ( *userData )->stagedWidget = widget;
            return 1;
          } else {
            Log::getInstance().warn( "TabPseudoElement::lua_create", "<tab> pseudo-element does not contain a child widget." );
          }
        } else {
          Log::getInstance().warn( "TabPseudoElement::lua_create", "This is not a <tab> pseudo-element." );
        }

        return 0;
      }

      int TabPseudoElement::lua_add( lua_State* L ) {
        Log::getInstance().warn( "TabPseudoElement::lua_add", "Cannot add elements or pseudo-elements to <tab> pseudo-element; use set_content to set single child." );
        return 0;
      }

      int TabPseudoElement::lua_removeWidget( lua_State* L ) {
        Log::getInstance().warn( "TabPseudoElement::lua_removeWidget", "Cannot remove elements or pseudo-elements from <tab> pseudo-element." );
        return 0;
      }

      int TabPseudoElement::lua_getName( lua_State* L ) {
        lua_pushstring( L, "tab" );

        return 1;
      }

      /**
       *
       * STACK ARGS: none
       * Returns: userdata, or nothing if a userdata cannot be created.
       */
      bool TabPseudoElement::getChild( lua_State* L ) {
        if( std::shared_ptr< sfg::Widget > tabLabel = subject->GetNthTabLabel( pageNumber ) ) {
          LuaElement::getUserdataFromWidget( L, tabLabel ); // userdata
          return true;
        } else {
          Log::getInstance().warn( "TabPseudoElement::getChild", "Tab has no label widget." );
          return false;
        }
      }

      void TabPseudoElement::setChild( lua_State* L, LuaElement* element ) {
        if( std::shared_ptr< sfg::Widget > existingPage = subject->GetNthPage( pageNumber ) ) {
          subject->SetTabLabel( existingPage, element->widget );
        } else {
          Log::getInstance().warn( "TabPseudoElement::setChild", "Tab has no label widget." );
        }
      }

      void TabPseudoElement::setChild( lua_State* L, const std::string& xmlString ) {
        if( std::shared_ptr< sfg::Widget > existingPage = subject->GetNthPage( pageNumber ) ) {
          WidgetBuilder widgetBuilder( displayState.instance.eventManager, displayState.getImageCache() );
          std::shared_ptr< sfg::Widget > child = nullptr;

          try {
            child = widgetBuilder.getWidgetFromXML( xmlString );

            subject->SetTabLabel( existingPage, child );
          } catch( std::exception& e ) {
            Log::getInstance().error( "LuaElement::add", "Failed to add widget XML: " + std::string( e.what() ) );
            return;
          }
        } else {
          Log::getInstance().warn( "TabPseudoElement::setChild", "Tab has no label widget." );
        }
      }

      int TabPseudoElement::lua_findElement( lua_State* L ) {
        Log::getInstance().warn( "TabPseudoElement::lua_findElement", "<tab> pseudo-element has no pseudo-element children." );
        return 0;
      }

      int TabPseudoElement::lua_getProperty( lua_State* L ) {
        Log::getInstance().warn( "TabPseudoElement::lua_getProperty", "<tab> pseudo-element has no gettable properties." );
        return 0;
      }

      int TabPseudoElement::lua_setProperty( lua_State* L ) {
        Log::getInstance().warn( "TabPseudoElement::lua_setProperty", "<tab> pseudo-element has no settable properties." );
        return 0;
      }

      int TabPseudoElement::lua_getContent( lua_State* L ) {
        TabPseudoElement* self = *( ( TabPseudoElement** ) luaL_checkudata( L, 1, "bluebear_tab_pseudo_element" ) );

        return self->getChild( L ) ? 1 : 0;
      }

      int TabPseudoElement::lua_setContent( lua_State* L ) {
        // FIXME: SFGUI bug. When you set the label on a tab widget, the tab becomes distorted.
        Log::getInstance().error( "TabPseudoElement::lua_setContent", "Cannot set <page> element after it is created due to a library/platform issue." );

        /*
        TabPseudoElement* self = *( ( TabPseudoElement** ) luaL_checkudata( L, 1, "bluebear_tab_pseudo_element" ) );

        if( lua_isstring( L, -1 ) ) {
          self->setChild( L, lua_tostring( L, -1 ) );
        } else {
          LuaElement* element = *( ( LuaElement** ) luaL_checkudata( L, 2, "bluebear_widget" ) );
          self->setChild( L, element );
        }
        */

        return 0;
      }

      int TabPseudoElement::lua_gc( lua_State* L ) {
        TabPseudoElement* widgetPtr = *( ( TabPseudoElement** ) luaL_checkudata( L, 1, "bluebear_tab_pseudo_element" ) );

        delete widgetPtr;

        return 0;
      }


    }
  }
}
