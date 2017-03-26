#ifndef TABPSEUDOELEMENT
#define TABPSEUDOELEMENT

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
      class LuaElement;

      class TabPseudoElement : public LuaPseudoElement {
        std::shared_ptr< sfg::Notebook > subject;
        unsigned int pageNumber;
        Display::MainGameState& displayState;

        bool getChild( lua_State* L );
        void setChild( lua_State* L, LuaElement* element );
        void setChild( lua_State* L, const std::string& xmlString );

      public:
        TabPseudoElement(
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
