#ifndef LUAELEMENT
#define LUAELEMENT

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <memory>
#include <SFGUI/Widgets.hpp>

namespace BlueBear {
  namespace Graphics {
    namespace GUI {
      // Reserve space for additional elements we may need for a LuaElement type.
      class LuaElement {
      public:
        std::shared_ptr< sfg::Widget > widget;

        static int lua_onEvent( lua_State* L );
      };
    }
  }
}


#endif
