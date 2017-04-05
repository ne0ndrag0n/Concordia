#ifndef LUAPSEUDOELEMENT
#define LUAPSEUDOELEMENT

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <string>

namespace BlueBear {
  namespace Graphics {
    namespace GUI {

      class LuaPseudoElement {
      public:
        virtual ~LuaPseudoElement() = default;
        virtual void setMetatable( lua_State* L ) = 0;
        virtual std::string getName() = 0;
      };

    }
  }
}

#endif
