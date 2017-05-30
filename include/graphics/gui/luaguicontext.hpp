#ifndef LUA_GUI_CONTEXT
#define LUA_GUI_CONTEXT

#include "graphics/display.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <SFGUI/Widgets.hpp>
#include <SFGUI/Desktop.hpp>
#include <memory>
#include <vector>
#include <set>
#include <tinyxml2.h>

namespace BlueBear {
  class EventManager;

  namespace Graphics {
    class WidgetBuilder;
    class ImageCache;

    namespace GUI {

      class LuaGUIContext {
        Display::MainGameState& displayState;

        /**
         * Bookkeeping for items belonging exclusively to this LuaGUIContext
         * This should NEVER have a nullptr added to it, EVER!!
         */
        std::set< std::weak_ptr< sfg::Widget >, std::owner_less< std::weak_ptr< sfg::Widget > > > myItems;

      public:
        LuaGUIContext( Display::MainGameState& displayState );
        ~LuaGUIContext();

        void addFromPath( const std::string& path );

        void add( std::shared_ptr< sfg::Widget > widget, bool toDesktop = false );
        void add( const std::string& xmlString );
        void remove( std::shared_ptr< sfg::Widget > widget );
        void removeAll();
        bool create( lua_State* L, const std::string& xmlString );
        std::shared_ptr< sfg::Widget > findById( const std::string& id );
        std::vector< std::shared_ptr< sfg::Widget > > findByClass( const std::string& clss );

        // TODO: Lua interfaces (these should be a lot less shitty than the way I did it in LuaElement)
        static int lua_add( lua_State* L );
        static int lua_createWidget( lua_State* L );
        static int lua_removeWidget( lua_State* L );
        static int lua_findById( lua_State* L );
        static int lua_findByClass( lua_State* L );
        static int lua_gc( lua_State* L );
      };

    }
  }
}


#endif
