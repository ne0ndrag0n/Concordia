#ifndef LUAELEMENT
#define LUAELEMENT

#include "bbtypes.hpp"
#include "graphics/display.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <memory>
#include <SFGUI/Widgets.hpp>
#include <map>
#include <unordered_map>
#include <set>

namespace BlueBear {
  class EventManager;

  namespace Graphics {
    class ImageCache;

    namespace GUI {
      class ItemPseudoElement;
      class PagePseudoElement;
      class RowPseudoElement;

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

        static std::map< std::weak_ptr< sfg::Widget >, std::map< sfg::Signal::SignalID, SignalBinding >, std::owner_less< std::weak_ptr< sfg::Widget > > > masterSignalMap;
        static std::map< std::weak_ptr< sfg::Widget >, std::map< std::string, std::string >, std::owner_less< std::weak_ptr< sfg::Widget > > > masterAttrMap;

        /**
         * Over time, make more of this shit a goddamn instance function!!
         */
        void add( lua_State* L, const std::string& xmlString, Display::MainGameState& state, int index = -1 );
        void add( LuaElement* element, int index );
        void add( lua_State* L, PagePseudoElement* page, int index );
        void add( lua_State* L, ItemPseudoElement* item, int index );
        void add( lua_State* L, RowPseudoElement* row );
        void addToCheckedContainer( std::shared_ptr< sfg::Widget > target, int position );
        void removeWidget( std::shared_ptr< sfg::Widget > target );
        void removePseudoElement( PagePseudoElement* page );
        void removePseudoElement( ItemPseudoElement* item );
        void removePseudoElement( RowPseudoElement* row );
        bool isContainer();
        bool getPseudoElements( lua_State* L, const std::string& pseudo, Display::MainGameState& state, int index = -1 );
        void operateTableAttribute(
          std::function< void( sfg::priv::TableCell&, std::shared_ptr< sfg::Table > ) > tableOp,
          std::function< void() > nonTableOp
        );

        static int lua_getName( lua_State* L );
        static int lua_onEvent( lua_State* L );
        static int lua_offEvent( lua_State* L );
        static int lua_getWidgetByID( lua_State* L );
        static int lua_getWidgetsByClass( lua_State* L );
        static int lua_getPseudoElements( lua_State* L );
        static int lua_gc( lua_State* L );
        static int lua_getText( lua_State* L );
        static int lua_setText( lua_State* L );
        static int lua_setImage( lua_State* L );
        static int lua_getProperty( lua_State* L );
        static int lua_setProperty( lua_State* L );
        static int lua_add( lua_State* L );
        static int lua_widgetRemove( lua_State* L );
        static int lua_getStyleProperty( lua_State* L );

        static void elementsToTable( lua_State* L, std::vector< std::shared_ptr< sfg::Widget > >& widgetList );
        static void setKeyboardStatus( lua_State* L );
        static void getUserdataFromWidget( lua_State* L, std::shared_ptr< sfg::Widget > widget );
        static void registerGenericHandler(
          lua_State* L,
          std::shared_ptr< sfg::Widget > widget,
          sfg::Signal::SignalID signalID
        );
        static void clickHandler(
          lua_State* L,
          std::weak_ptr< sfg::Widget > selfElement,
          LuaReference masterReference,
          const std::string& buttonTag
        );
        static void keyHandler(
          lua_State* L,
          std::weak_ptr< sfg::Widget > selfElement,
          Display::MainGameState* state,
          LuaReference masterReference
        );
        static void genericHandler(
          lua_State* L,
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

        static std::list< sfg::priv::TableCell >::iterator getCell( std::list< sfg::priv::TableCell >& tableCellList, std::shared_ptr< sfg::Widget > widget );

        static void setCustomAttribute( std::shared_ptr< sfg::Widget > widget, const std::string& key, const std::string& value );
        static std::string getCustomAttribute( std::shared_ptr< sfg::Widget > widget, const std::string& key );

        static void queryUnsignedAttribute( std::shared_ptr< sfg::Widget > widget, const std::string& key, unsigned int* value );
        static void queryFloatAttribute( std::shared_ptr< sfg::Widget > widget, const std::string& key, float* value );
        static void queryBoolAttribute( std::shared_ptr< sfg::Widget > widget, const std::string& key, bool* value );
        static bool propertyIsSet( std::shared_ptr< sfg::Widget > widget, const std::string& key );
        static void masterMapGC();

        template < typename T > static std::shared_ptr< sfg::Adjustment > getAdjustment( std::shared_ptr< sfg::Widget > widget ) {
          std::shared_ptr< T > adjustmentType = std::static_pointer_cast< T >( widget );
          return adjustmentType->GetAdjustment();
        };

        static void setId( std::shared_ptr< sfg::Widget > widget, const std::string& id );
        static void setClass( std::shared_ptr< sfg::Widget > widget, const std::string& clss );
        static std::string getId( std::shared_ptr< sfg::Widget > widget );
        static std::string getClass( std::shared_ptr< sfg::Widget > widget );
        static std::shared_ptr< sfg::Widget > getWidgetById( std::shared_ptr< sfg::Widget > parent, const std::string& id );
        static std::vector< std::shared_ptr< sfg::Widget > > getWidgetsByClass( std::shared_ptr< sfg::Widget > parent, const std::string& clss );
        static std::vector< std::shared_ptr< sfg::Widget > > getWidgetsByName( std::shared_ptr< sfg::Widget > parent, const std::string& name );
      };
    }
  }
}


#endif
