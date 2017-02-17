#ifndef LUAELEMENT
#define LUAELEMENT

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

namespace BlueBear {
  namespace Graphics {

    // Reserve space for additional elements we may need for a LuaElement type.
    class LuaElement {
    public:
      std::shared_ptr< sfg::Widget > widget;

      static int lua_onEvent( lua_State* L );
    };

  }
}


#endif
