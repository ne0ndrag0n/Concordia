#include "graphics/gui/luapseudoelement/pagepseudoelement.hpp"
#include "graphics/gui/luapseudoelement/tabpseudoelement.hpp"
#include "tools/ctvalidators.hpp"
#include "tools/utility.hpp"
#include "log.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace GUI {

      PagePseudoElement::PagePseudoElement( std::shared_ptr< sfg::Notebook > subject, unsigned int pageNumber, Display::MainGameState& displayState )
        : subject( subject ), pageNumber( pageNumber ), displayState( displayState ) {}

      /**
       *
       * STACK ARGS: userdata
       * (Stack is unmodified after call)
       */
      void PagePseudoElement::setMetatable( lua_State* L ) {
        if( luaL_newmetatable( L, "bluebear_page_pseudo_element" ) ) { // metatable userdata
          luaL_Reg tableFuncs[] = {
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

      int PagePseudoElement::lua_findElement( lua_State* L ) {
        VERIFY_STRING( "PagePseudoElement::lua_findElement", "find_pseudo" );

        PagePseudoElement* self = *( ( PagePseudoElement** ) luaL_checkudata( L, 1, "bluebear_page_pseudo_element" ) );

        // <page> <tab /> <content /> </page>
        return self->findElement( L, lua_tostring( L, -1 ) ) ? 1 : 0;
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

        delete widgetPtr;

        return 0;
      }

    }
  }
}
