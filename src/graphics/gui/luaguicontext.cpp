#include "graphics/gui/luaguicontext.hpp"
#include "graphics/gui/luaelement.hpp"
#include "graphics/gui/sfgroot.hpp"
#include "graphics/imagecache.hpp"
#include "graphics/widgetbuilder.hpp"
#include "eventmanager.hpp"

#define LuaGUIContext_VERIFYSTRING( tag, func ) \
            if( !lua_isstring( L, -1 ) ) { \
              Log::getInstance().warn( tag, "Argument 1 provided to " func " must be a string." ); \
              return 0; \
            }


namespace BlueBear {
  namespace Graphics {
    namespace GUI {

      LuaGUIContext::LuaGUIContext( sfg::Desktop& desktop, EventManager& eventManager, ImageCache& imageCache )
        : rootContainer( RootContainer::Create() ), desktop( desktop ), eventManager( eventManager ), imageCache( imageCache ) {}

      void LuaGUIContext::addFromPath( const std::string& path ) {
        WidgetBuilder widgetBuilder( eventManager, imageCache );
        std::vector< std::shared_ptr< sfg::Widget > > widgets = widgetBuilder.getWidgets( path );

        for( auto& widget : widgets ) {
          add( widget, true );
        }
      }

      void LuaGUIContext::add( std::shared_ptr< sfg::Widget > widget, bool toDesktop ) {
        rootContainer->Add( widget );

        if( toDesktop ) {
          desktop.Add( widget );

          addedToDesktop.insert( widget );
        }
      }

      void LuaGUIContext::remove( std::shared_ptr< sfg::Widget > widget ) {
        rootContainer->Remove( widget );

        if( addedToDesktop.find( widget ) != addedToDesktop.end() ) {
          desktop.Remove( widget );
          addedToDesktop.erase( widget );
        }
      }

      /**
       * Use this function to basically wipe everything off the screen
       */
      void LuaGUIContext::removeAll() {
        // Remove desktopped items (this is where items will be removed from the screen)
        for( auto& item : addedToDesktop ) {
          desktop.Remove( item );
        }

        // Free all elements within container
        rootContainer = RootContainer::Create();

        addedToDesktop.clear();
      }

      std::shared_ptr< sfg::Widget > LuaGUIContext::findById( const std::string& id ) {
        return rootContainer->GetWidgetById( id );
      }

      std::vector< std::shared_ptr< sfg::Widget > > LuaGUIContext::findByClass( const std::string& clss ) {
        return rootContainer->GetWidgetsByClass( clss );
      }

      int LuaGUIContext::lua_findById( lua_State* L ) {
        LuaGUIContext_VERIFYSTRING( "LuaGUIContext::lua_findById", "find_by_id" );
        LuaGUIContext* self = *( ( LuaGUIContext** ) luaL_checkudata( L, 1, "bluebear_gui_context" ) );

        std::string selector( lua_tostring( L, -1 ) );
        if( std::shared_ptr< sfg::Widget > widget = self->findById( selector ) ) {
          LuaElement::getUserdataFromWidget( L, widget ); // userdata "selector"
          return 1;
        } else {
          std::string error = std::string( "Could not find widget with ID " ) + selector;
          return luaL_error( L, error.c_str() );
        }
      }

      int LuaGUIContext::lua_findByClass( lua_State* L ) {
        LuaGUIContext_VERIFYSTRING( "LuaGUIContext::lua_findByClass", "find_by_class" );
        LuaGUIContext* self = *( ( LuaGUIContext** ) luaL_checkudata( L, 1, "bluebear_gui_context" ) );

        std::string selector( lua_tostring( L, -1 ) );
        std::vector< std::shared_ptr< sfg::Widget > > widgets = self->findByClass( selector );
        auto size = widgets.size();

        if( size ) {
          // At least one widget is present - these items must be chained in a Lua table
          // Create a Lua table, then push a new LuaInstance wrapper for every widget found as part of this class
          lua_createtable( L, size, 0 ); // table "selector"

          for( int i = 0; i != size; i++ ) {
            LuaElement::getUserdataFromWidget( L, widgets[ i ] ); // userdata table "selector"
            lua_rawseti( L, -2, i + 1 ); // table "selector"
          }

          return 1;
        } else {
          std::string error = std::string( "Could not find any widgets with class " ) + selector;
          return luaL_error( L, error.c_str() );
        }
      }

      int LuaGUIContext::lua_gc( lua_State* L ) {
        LuaGUIContext* ptr = *( ( LuaGUIContext** ) luaL_checkudata( L, 1, "bluebear_gui_context" ) );

        delete ptr;

        return 0;
      }
    }
  }
}
