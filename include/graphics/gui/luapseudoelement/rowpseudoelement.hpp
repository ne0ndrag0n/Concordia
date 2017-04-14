#ifndef ROWPSEUDOELEMENT
#define ROWPSEUDOELEMENT

#include "graphics/gui/luapseudoelement/luapseudoelement.hpp"
#include "graphics/display.hpp"
#include <string>
#include <memory>
#include <SFGUI/Widgets.hpp>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <tinyxml2.h>

namespace BlueBear {
  namespace Graphics {
    namespace GUI {

      class RowPseudoElement : public LuaPseudoElement {
        std::shared_ptr< sfg::Table > subject;

      };

    }
  }
}


#endif
