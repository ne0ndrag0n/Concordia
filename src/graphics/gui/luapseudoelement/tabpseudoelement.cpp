#include "graphics/gui/luapseudoelement/tabpseudoelement.hpp"
#include "log.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace GUI {

      TabPseudoElement::TabPseudoElement( std::shared_ptr< sfg::Notebook > subject ) : subject( subject ) {}

      /**
       *
       * STACK ARGS: userdata
       * (Stack is unmodified after call)
       */
      void TabPseudoElement::setMetatable( lua_State* L ) {
        if( luaL_newmetatable( L, "bluebear_page_pseudo_element" ) ) { // metatable userdata
          luaL_Reg tableFuncs[] = {
            { "find_pseudo", TabPseudoElement::lua_findElement },
            { "get_property", TabPseudoElement::lua_getProperty },
            { "set_property", TabPseudoElement::lua_setProperty },
            { "get_content", TabPseudoElement::lua_getContent },
            { "set_content", TabPseudoElement::lua_setContent },
            { NULL, NULL }
          };

          luaL_setfuncs( L, tableFuncs, 0 );

          lua_pushvalue( L, -1 ); // metatable metatable userdata

          lua_setfield( L, -2, "__index" ); // metatable userdata
        }

        lua_setmetatable( L, -2 ); // userdata
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
        return 0;
      }

      int TabPseudoElement::lua_setContent( lua_State* L ) {
        return 0;
      }

    }
  }
}
