#ifndef NBBINPSEUDOELEMENT
#define NBBINPSEUDOELEMENT

#include "graphics/gui/luapseudoelement/luapseudoelement.hpp"
#include "graphics/display.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <SFGUI/Widgets.hpp>
#include <memory>
#include <tinyxml2.h>

namespace BlueBear {
  namespace Graphics {
    namespace GUI {
      class LuaElement;

      class NBBinPseudoElement : public LuaPseudoElement {
      protected:
        std::shared_ptr< sfg::Notebook > subject;
        unsigned int pageNumber;
        Display::MainGameState& displayState;

        void setStagedChild( lua_State* L, LuaElement* element );
        void setStagedChild( lua_State* L, const std::string& xmlString );

      public:
        std::shared_ptr< sfg::Widget > stagedWidget;

        NBBinPseudoElement(
          std::shared_ptr< sfg::Notebook > subject,
          unsigned int pageNumber,
          Display::MainGameState& displayState
        );

        std::string getName();

        void setMetatable( lua_State* L );
        virtual void setSubject( std::shared_ptr< sfg::Notebook > subject, unsigned int pageNumber );

        int getElementsByClass( lua_State* L, const std::string& classID );
        int getElementById( lua_State* L, const std::string& id );

        std::shared_ptr< sfg::Widget > getChildWidget();
        virtual std::shared_ptr< sfg::Widget > getSubjectChildWidget() = 0;

        static std::shared_ptr< sfg::Widget > createStagedWidget( Display::MainGameState& displayState, tinyxml2::XMLElement* element );

        static int lua_add( lua_State* L );
        static int lua_removeWidget( lua_State* L );
        static int lua_getName( lua_State* L );
        static int lua_findById( lua_State* L );
        static int lua_findByClass( lua_State* L );
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
