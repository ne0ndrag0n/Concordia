#ifndef TABPSEUDOELEMENT
#define TABPSEUDOELEMENT

#include "graphics/gui/luapseudoelement/luapseudoelement.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <SFGUI/Widgets.hpp>
#include <memory>

namespace BlueBear {
  namespace Graphics {
    namespace GUI {

      class TabPseudoElement : public LuaPseudoElement {
        std::shared_ptr< sfg::Notebook > subject;

      public:
        TabPseudoElement( std::shared_ptr< sfg::Notebook > subject );

        void setMetatable( lua_State* L );

        static int lua_findElement( lua_State* L );
        static int lua_getProperty( lua_State* L );
        static int lua_setProperty( lua_State* L );
        static int lua_getContent( lua_State* L );
        static int lua_setContent( lua_State* L );
      };

    }
  }
}

#endif
