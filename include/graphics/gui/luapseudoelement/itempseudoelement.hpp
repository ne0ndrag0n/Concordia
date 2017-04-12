#ifndef ITEMPSEUDOELEMENT
#define ITEMPSEUDOELEMENT

#include "graphics/gui/luapseudoelement/luapseudoelement.hpp"
#include <string>
#include <memory>
#include <SFGUI/Widgets.hpp>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

namespace BlueBear {
  namespace Graphics {
    namespace GUI {

      class ItemPseudoElement : public LuaPseudoElement {
        std::shared_ptr< sfg::ComboBox > subject;
        int elementNumber;

        std::string stagedItem;

      public:
        ItemPseudoElement(
          std::shared_ptr< sfg::ComboBox > subject,
          int elementNumber
        );

        void setMetatable( lua_State* L );
        std::string getName();

        void setItem( const std::string& content );
        std::string getItem();

        static int lua_gc( lua_State* L );
        static int lua_add( lua_State* L );
        static int lua_removeWidget( lua_State* L );
        static int lua_getName( lua_State* L );
        static int lua_findElement( lua_State* L );
        static int lua_findBy( lua_State* L );
        static int lua_property( lua_State* L );
        static int lua_getContent( lua_State* L );
        static int lua_setContent( lua_State* L );
      };

    }
  }
}



#endif
