#include "graphics/gui/luaguicontext.hpp"
#include "graphics/gui/luaelement.hpp"
#include "graphics/gui/sfgroot.hpp"
#include "graphics/gui/luapseudoelement/pagepseudoelement.hpp"
#include "graphics/gui/luapseudoelement/nbbinpseudoelement.hpp"
#include "graphics/gui/luapseudoelement/tabpseudoelement.hpp"
#include "graphics/gui/luapseudoelement/contentpseudoelement.hpp"
#include "graphics/gui/luapseudoelement/itempseudoelement.hpp"
#include "graphics/gui/luapseudoelement/rowpseudoelement.hpp"
#include "graphics/imagecache.hpp"
#include "graphics/widgetbuilder.hpp"
#include "tools/utility.hpp"
#include "tools/ctvalidators.hpp"
#include "eventmanager.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace GUI {

      LuaGUIContext::LuaGUIContext( Display::MainGameState& displayState )
        : displayState( displayState ) {
        eventManager.CONTEXT_REQUEST.listen( this, [ & ]( std::shared_ptr< sfg::Widget > widget ) {
          // not end = the item (or the item's most distant parent) belongs to this context
          return myItems.find( Tools::Utility::getWidgetOrAncestor( widget ) ) != myItems.end();
        } );
      }

      LuaGUIContext::~LuaGUIContext() {
        eventManager.CONTEXT_REQUEST.stopListening( this );
      }

      void LuaGUIContext::addFromPath( const std::string& path ) {
        WidgetBuilder widgetBuilder( displayState.getImageCache() );
        std::vector< std::shared_ptr< sfg::Widget > > widgets = widgetBuilder.getWidgets( path );

        for( auto& widget : widgets ) {
          add( widget, true );
        }
      }

      void LuaGUIContext::add( std::shared_ptr< sfg::Widget > widget, bool toDesktop ) {
        if( toDesktop ) {
          displayState.gui.desktop.Add( widget );

          myItems.insert( widget );

          LuaElement::updateAncestorPrefixes( widget );
        }
      }

      void LuaGUIContext::add( const std::string& xmlString ) {
        WidgetBuilder widgetBuilder( displayState.getImageCache() );

        try {
          add( widgetBuilder.getWidgetFromXML( xmlString ), true );
        } catch( std::exception& e ) {
          Log::getInstance().error( "LuaGUIContext::add", "Failed to add widget XML: " + std::string( e.what() ) );
        }
      }

      void LuaGUIContext::remove( std::shared_ptr< sfg::Widget > widget ) {
        widget->Show( false );

        if( myItems.find( widget ) != myItems.end() ) {
          displayState.gui.desktop.Remove( widget );
          myItems.erase( widget );
        }

        LuaElement::updateAncestorPrefixes( widget );
      }

      /**
       * Use this function to basically wipe everything off the screen
       */
      void LuaGUIContext::removeAll() {
        // Remove desktopped items (this is where items will be removed from the screen)
        for( std::weak_ptr< sfg::Widget > pointer : myItems ) {
          if( std::shared_ptr< sfg::Widget > item = pointer.lock() ) {
            item->Show( false );
            displayState.gui.desktop.Remove( item );

            LuaElement::updateAncestorPrefixes( item );
          }
        }

        myItems.clear();
      }

      std::shared_ptr< sfg::Widget > LuaGUIContext::findById( const std::string& id ) {
        std::shared_ptr< sfg::Widget > result = sfg::Widget::GetWidgetById( Tools::Utility::pointerToString( this ) + ":" + id );

        if( myItems.find( Tools::Utility::getWidgetOrAncestor( result ) ) != myItems.end() ) {
          return result;
        } else {
          return nullptr;
        }
      }

      std::vector< std::shared_ptr< sfg::Widget > > LuaGUIContext::findByClass( const std::string& clss ) {
        std::vector< std::shared_ptr< sfg::Widget > > results = sfg::Widget::GetWidgetsByClass( Tools::Utility::pointerToString( this ) + ":" + clss );

        results.erase(
          std::remove_if(
            results.begin(),
            results.end(),
            [ & ]( std::shared_ptr< sfg::Widget > widget ) {
              return myItems.find( Tools::Utility::getWidgetOrAncestor( widget ) ) == myItems.end();
            }
          ),
          results.end()
        );

        return results;
      }

      bool LuaGUIContext::create( lua_State* L, const std::string& xmlString ) {
        tinyxml2::XMLDocument document;
        document.Parse( xmlString.c_str() );

        if( document.ErrorID() ) {
          Log::getInstance().error( "LuaGUIContext::create", "Could not parse XML string!" );
          return false;
        }

        tinyxml2::XMLElement* element = document.RootElement();
        if( !element ) {
          Log::getInstance().error( "LuaGUIContext::create", "No root element!" );
          return false;
        }

        switch( Tools::Utility::hash( element->Name() ) ) {
          case Tools::Utility::hash( "tab" ):
            return TabPseudoElement::create( L, displayState, element ) == 1 ? true : false; // userdata
          case Tools::Utility::hash( "content" ):
            return ContentPseudoElement::create( L, displayState, element ) == 1 ? true : false; // userdata
          case Tools::Utility::hash( "page" ):
            return PagePseudoElement::create( L, displayState, element ) == 1 ? true : false; // userdata
          case Tools::Utility::hash( "item" ):
            return ItemPseudoElement::create( L, displayState, element ) == 1 ? true : false; // userdata
          case Tools::Utility::hash( "row" ):
            return RowPseudoElement::create( L, displayState, element ) == 1 ? true : false; // userdata
          default:
            try {
              WidgetBuilder widgetBuilder( displayState.getImageCache() );

              std::shared_ptr< sfg::Widget > widget = widgetBuilder.getWidgetFromElementDirect( element );
              LuaElement::updateAncestorPrefixes( widget );
              LuaElement::getUserdataFromWidget( L, widget ); // userdata
              return true;
            } catch( std::exception& e ) {
              Log::getInstance().error( "LuaGUIContext::create", "Failed to add widget XML: " + std::string( e.what() ) );
              return false;
            }
        }
      }

      int LuaGUIContext::lua_add( lua_State* L ) {
        LuaGUIContext* self = *( ( LuaGUIContext** ) luaL_checkudata( L, 1, "bluebear_gui_context" ) );

        if( lua_isstring( L, -1 ) ) {
          self->add( lua_tostring( L, -1 ) );
        } else {
          // Will throw exception if this does not match
          LuaElement* argument = *( ( LuaElement** ) luaL_checkudata( L, 2, "bluebear_widget" ) );
          self->add( argument->widget, true );
        }

        return 0;
      }

      int LuaGUIContext::lua_createWidget( lua_State* L ) {
        VERIFY_STRING( "LuaGUIContext::lua_createWidget", "create" );
        LuaGUIContext* self = *( ( LuaGUIContext** ) luaL_checkudata( L, 1, "bluebear_gui_context" ) );

        if( self->create( L, lua_tostring( L, -1 ) ) ) { // userdata
          return 1;
        } else {
          return 0;
        }
      }

      int LuaGUIContext::lua_removeWidget( lua_State* L ) {
        VERIFY_USER_DATA( "LuaGUIContext::lua_removeWidget", "remove" );

        LuaGUIContext* self = *( ( LuaGUIContext** ) luaL_checkudata( L, 1, "bluebear_gui_context" ) );
        LuaElement* argument = *( ( LuaElement** ) luaL_checkudata( L, 2, "bluebear_widget" ) );

        self->remove( argument->widget );

        return 0;
      }

      int LuaGUIContext::lua_findById( lua_State* L ) {
        VERIFY_STRING( "LuaGUIContext::lua_findById", "find_by_id" );
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
        VERIFY_STRING( "LuaGUIContext::lua_findByClass", "find_by_class" );
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
