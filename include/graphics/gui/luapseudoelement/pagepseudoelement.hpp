#ifndef PAGEPSEUDOELEMENT
#define PAGEPSEUDOELEMENT

#include "graphics/gui/luapseudoelement/luapseudoelement.hpp"
#include "graphics/display.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <SFGUI/Widgets.hpp>
#include <memory>

namespace BlueBear {
  namespace Graphics {
    namespace GUI {

      class PagePseudoElement : public LuaPseudoElement {
        std::shared_ptr< sfg::Notebook > subject;
        unsigned int pageNumber;
        Display::MainGameState& displayState;

        bool findElement( lua_State* L, const std::string& tag );

      public:
        PagePseudoElement(
          std::shared_ptr< sfg::Notebook > subject,
          unsigned int pageNumber,
          Display::MainGameState& displayState
        );

        void setMetatable( lua_State* L );

        static int lua_findElement( lua_State* L );
        static int lua_getProperty( lua_State* L );
        static int lua_setProperty( lua_State* L );
        static int lua_getContent( lua_State* L );
        static int lua_setContent( lua_State* L );
        static int lua_gc( lua_State* L );
      };

    }
  }
}


#endif
