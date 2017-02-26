#ifndef LUAELEMENT
#define LUAELEMENT

#include "bbtypes.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <memory>
#include <SFGUI/Widgets.hpp>
#include <map>

namespace BlueBear {
  class EventManager;

  namespace Graphics {
    namespace GUI {
      // Reserve space for additional elements we may need for a LuaElement type.

      /**
       * This is a Luasphere object that wraps an SFGUI object. It is only a wrapper, and does
       * not signify an actual equality to a widget (unless the __eq metamethod is overridden)
       */
      class LuaElement {
      public:
        std::shared_ptr< sfg::Widget > widget;

        struct SignalBinding {
          LuaReference reference;
          unsigned int slotHandle;
        };

        /**
         * Much of this has to be static as a LuaElement is a functional wrapper around an sfg::Widget
         * as it relates to the Concordia GUI.
         */
        static std::map< void*, std::map< sfg::Signal::SignalID, SignalBinding > > masterSignalMap;
        static int lua_onEvent( lua_State* L );
        static int lua_offEvent( lua_State* L );
        static int lua_getWidgetByID( lua_State* L );
        static int lua_getWidgetsByClass( lua_State* L );
        static int lua_gc( lua_State* L );
        static int lua_getText( lua_State* L );
        static int lua_setText( lua_State* L );
        static int lua_setImage( lua_State* L );
        static int lua_getProperty( lua_State* L );
        static int lua_setProperty( lua_State* L );

        static void getUserdataFromWidget( lua_State* L, std::shared_ptr< sfg::Widget > widget );
        static void registerGenericHandler(
          lua_State* L,
          EventManager& eventManager,
          std::shared_ptr< sfg::Widget > widget,
          sfg::Signal::SignalID signalID
        );
        static void clickHandler(
          lua_State* L,
          EventManager& eventManager,
          std::weak_ptr< sfg::Widget > selfElement,
          LuaReference masterReference,
          const std::string& buttonTag
        );
        static void genericHandler(
          lua_State* L,
          EventManager& eventManager,
          std::weak_ptr< sfg::Widget > widgetPtr,
          LuaReference masterReference
        );
        static void unregisterClickHandler(
          lua_State* L,
          std::map< sfg::Signal::SignalID, LuaElement::SignalBinding >& signalMap,
          std::shared_ptr< sfg::Widget > widget
        );
        static void unregisterHandler(
          lua_State* L,
          std::map< sfg::Signal::SignalID, LuaElement::SignalBinding >& signalMap,
          std::shared_ptr< sfg::Widget > widget,
          sfg::Signal::SignalID signalID
        );
        static void getWindowProps(
          std::shared_ptr< sfg::Window > window,
          bool& titlebar,
          bool& background,
          bool& resize,
          bool& shadow,
          bool& close
        );
        static void setWindowProps(
          std::shared_ptr< sfg::Window > window,
          bool titlebar,
          bool background,
          bool resize,
          bool shadow,
          bool close
        );
      };
    }
  }
}


#endif
