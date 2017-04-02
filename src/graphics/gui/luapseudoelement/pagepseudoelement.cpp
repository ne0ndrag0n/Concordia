#include "graphics/gui/luapseudoelement/pagepseudoelement.hpp"
#include "graphics/gui/luapseudoelement/tabpseudoelement.hpp"
#include "scripting/luakit/gchelper.hpp"
#include "tools/ctvalidators.hpp"
#include "tools/utility.hpp"
#include "log.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace GUI {

      PagePseudoElement::PagePseudoElement( std::shared_ptr< sfg::Notebook > subject, unsigned int pageNumber, Display::MainGameState& displayState )
        : subject( subject ), pageNumber( pageNumber ), displayState( displayState ), stagedTabElement( nullptr ) {}

      /**
       *
       * STACK ARGS: userdata
       * (Stack is unmodified after call)
       */
      void PagePseudoElement::setMetatable( lua_State* L ) {
        if( luaL_newmetatable( L, "bluebear_page_pseudo_element" ) ) { // metatable userdata
          luaL_Reg tableFuncs[] = {
            { "add", PagePseudoElement::lua_add },
            { "remove", PagePseudoElement::lua_removeWidget },
            { "get_name", PagePseudoElement::lua_getName },
            { "find_pseudo", PagePseudoElement::lua_findElement },
            { "get_property", PagePseudoElement::lua_getProperty },
            { "set_property", PagePseudoElement::lua_setProperty },
            { "get_content", PagePseudoElement::lua_getContent },
            { "set_content", PagePseudoElement::lua_setContent },
            { "__gc", PagePseudoElement::lua_gc },
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
       * Stack is unmodified after call
       */
      void PagePseudoElement::processXMLPseudoElement( lua_State* L, tinyxml2::XMLElement* child ) {
        // <tab> pseudo-element: screen, create
        // <content> pseudo-element: screen, create
        // Other elements - warn and ignore
        std::string tag( child->Name() );

        switch( Tools::Utility::hash( tag.c_str() ) ) {
          case Tools::Utility::hash( "tab" ): {
            if( !this->stagedTabElement && TabPseudoElement::create( L, displayState, child ) ) { // tabuserdata
              this->setStagedTabElement( L, *( ( TabPseudoElement** ) lua_topointer( L, -1 ) ) );

              lua_pop( L, 1 ); // EMPTY
            } // EMPTY
            break;
          }
          case Tools::Utility::hash( "content" ): {
            Log::getInstance().error( "PagePseudoElement::processXMLPseudoElement", "Unimplemented" );
            break;
          }
          default:
            Log::getInstance().warn( "PagePseudoElement::processXMLPseudoElement", "Cannot add element of type " + tag + " to <page> pseudo-element; ignoring." );
        }

      }

      /**
       *
       * STACK ARGS: none
       * Returns: userdata
       */
      int PagePseudoElement::create( lua_State* L, Display::MainGameState& displayState, tinyxml2::XMLElement* element ) {
        PagePseudoElement** userData = ( PagePseudoElement** ) lua_newuserdata( L, sizeof( PagePseudoElement* ) ); // userdata
        *userData = new PagePseudoElement( nullptr, 0, displayState );
        PagePseudoElement* self = *userData;
        self->setMetatable( L );

        for( tinyxml2::XMLElement* child = element->FirstChildElement(); child != NULL; child = child->NextSiblingElement() ) {
          self->processXMLPseudoElement( L, child );
        }

        return 1;
      }

      /**
       *
       * STACK ARGS: userdata if stagedTabElement is not null
       * Stack is unmodified after call
       */
      void PagePseudoElement::setStagedTabElement( lua_State* L, TabPseudoElement* stagedTabElement ) {
        if( this->stagedTabElement ) {
          Scripting::LuaKit::GcHelper::untrack(
            L,
            Tools::Utility::pointerToString( this ) + ":" + Tools::Utility::pointerToString( this->stagedTabElement )
          );
        }

        if( this->stagedTabElement = stagedTabElement ) {
          Scripting::LuaKit::GcHelper::track(
            L,
            Tools::Utility::pointerToString( this ) + ":" + Tools::Utility::pointerToString( this->stagedTabElement )
          );
        }
      }

      int PagePseudoElement::lua_add( lua_State* L ) {
        Log::getInstance().warn( "PagePseudoElement::lua_add", "Cannot add elements or pseudo-elements to <page> pseudo-element." );
        return 0;
      }

      int PagePseudoElement::lua_removeWidget( lua_State* L ) {
        Log::getInstance().warn( "PagePseudoElement::lua_removeWidget", "Cannot remove elements or pseudo-elements from <page> pseudo-element." );
        return 0;
      }

      int PagePseudoElement::lua_getName( lua_State* L ) {
        lua_pushstring( L, "page" );

        return 1;
      }

      /**
       *
       * STACK ARGS: none
       * Returns: userdata, or nothing if a userdata cannot be created.
       */
      bool PagePseudoElement::findElement( lua_State* L, const std::string& tag ) {
        switch( Tools::Utility::hash( tag.c_str() ) ) {
          case Tools::Utility::hash( "tab" ):
            {
              TabPseudoElement** userData = ( TabPseudoElement** ) lua_newuserdata( L, sizeof( TabPseudoElement* ) ); // userdata
              *userData = new TabPseudoElement( subject, pageNumber, displayState );
              ( *userData )->setMetatable( L );
            }
            return true;
          case Tools::Utility::hash( "content" ):
            {
              Log::getInstance().error( "PagePseudoElement::findElement", "Unimplemented" );
              return false;
            }
            return true;
          default:
            Log::getInstance().warn( "PagePseudoElement::findElement", "<page> pseduo-element only contains pseudo-elements of type <tab> or <content>." );
        }

        return false;
      }

      /**
       *
       * STACK ARGS: none
       * Returns: userdata, or nothing if a userdata cannot be created.
       */
      int PagePseudoElement::findElementStaged( lua_State* L, const std::string& tag ) {
        switch( Tools::Utility::hash( tag.c_str() ) ) {
          case Tools::Utility::hash( "tab" ): {
            if( this->stagedTabElement ) {
              Scripting::LuaKit::GcHelper::getTracked(
                L,
                Tools::Utility::pointerToString( this ) + ":" + Tools::Utility::pointerToString( this->stagedTabElement )
              ); // userdata/nil

              return lua_isnil( L, -1 ) ? 0 : 1;
            } else {
              Log::getInstance().debug( "PagePseudoElement::findElementStaged", "No <tab> element present in this <page>." );
            }

            break;
          }
          case Tools::Utility::hash( "content" ): {
            Log::getInstance().error( "PagePseudoElement::findElementStaged", "Unimplemented" );
            break;
          }
          default:
            Log::getInstance().warn( "PagePseudoElement::findElementStaged", "<page> pseduo-element only contains pseudo-elements of type <tab> or <content>." );
        }

        return 0;
      }

      int PagePseudoElement::lua_findElement( lua_State* L ) {
        VERIFY_STRING( "PagePseudoElement::lua_findElement", "find_pseudo" );

        PagePseudoElement* self = *( ( PagePseudoElement** ) luaL_checkudata( L, 1, "bluebear_page_pseudo_element" ) );

        // <page> <tab /> <content /> </page>
        if( self->subject ) {
          return self->findElement( L, lua_tostring( L, -1 ) ) ? 1 : 0; // userdata or none
        } else {
          return self->findElementStaged( L, lua_tostring( L, -1 ) ); // userdata or none
        }
      }

      int PagePseudoElement::lua_getProperty( lua_State* L ) {
        Log::getInstance().warn( "PagePseudoElement::lua_getProperty", "<page> pseudo-element has no gettable properties." );
        return 0;
      }

      int PagePseudoElement::lua_setProperty( lua_State* L ) {
        Log::getInstance().warn( "PagePseudoElement::lua_setProperty", "<page> pseudo-element has no settable properties." );
        return 0;
      }

      int PagePseudoElement::lua_getContent( lua_State* L ) {
        Log::getInstance().warn( "PagePseudoElement::lua_getContent", "<page> pseudo-element has no direct content." );
        return 0;
      }

      int PagePseudoElement::lua_setContent( lua_State* L ) {
        Log::getInstance().warn( "PagePseudoElement::lua_setContent", "<page> pseudo-element has no direct content." );
        return 0;
      }

      int PagePseudoElement::lua_gc( lua_State* L ) {
        PagePseudoElement* widgetPtr = *( ( PagePseudoElement** ) luaL_checkudata( L, 1, "bluebear_page_pseudo_element" ) );

        widgetPtr->setStagedTabElement( L, nullptr );

        delete widgetPtr;

        return 0;
      }

    }
  }
}
