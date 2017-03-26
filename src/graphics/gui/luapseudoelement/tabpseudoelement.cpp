#include "graphics/gui/luapseudoelement/tabpseudoelement.hpp"
#include "graphics/gui/luaelement.hpp"
#include "graphics/widgetbuilder.hpp"
#include "tools/ctvalidators.hpp"
#include "log.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace GUI {

      TabPseudoElement::TabPseudoElement( std::shared_ptr< sfg::Notebook > subject, unsigned int pageNumber, Display::MainGameState& displayState )
        : subject( subject ), pageNumber( pageNumber ), displayState( displayState ) {}

      /**
       *
       * STACK ARGS: userdata
       * (Stack is unmodified after call)
       */
      void TabPseudoElement::setMetatable( lua_State* L ) {
        if( luaL_newmetatable( L, "bluebear_tab_pseudo_element" ) ) { // metatable userdata
          luaL_Reg tableFuncs[] = {
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
        if( std::shared_ptr< sfg::Widget > existingTabLabel = subject->GetNthTabLabel( pageNumber ) ) {
          subject->SetTabLabel( existingTabLabel, element->widget );
        } else {
          Log::getInstance().warn( "TabPseudoElement::setChild", "Tab has no label widget." );
        }
      }

      void TabPseudoElement::setChild( lua_State* L, const std::string& xmlString ) {
        if( std::shared_ptr< sfg::Widget > existingTabLabel = subject->GetNthTabLabel( pageNumber ) ) {
          WidgetBuilder widgetBuilder( displayState.instance.eventManager, displayState.getImageCache() );
          std::shared_ptr< sfg::Widget > child = nullptr;

          try {
            child = widgetBuilder.getWidgetFromXML( xmlString );

            subject->SetTabLabel( existingTabLabel, child );
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
        TabPseudoElement* self = *( ( TabPseudoElement** ) luaL_checkudata( L, 1, "bluebear_tab_pseudo_element" ) );

        if( lua_isstring( L, -1 ) ) {
          self->setChild( L, lua_tostring( L, -1 ) );
        } else {
          LuaElement* element = *( ( LuaElement** ) luaL_checkudata( L, 2, "bluebear_widget" ) );
          self->setChild( L, element );
        }

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
