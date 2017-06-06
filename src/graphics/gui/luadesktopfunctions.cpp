#include "graphics/gui/luadesktopfunctions.hpp"
#include "graphics/gui/luapseudoelement/pagepseudoelement.hpp"
#include "graphics/gui/luapseudoelement/nbbinpseudoelement.hpp"
#include "graphics/gui/luapseudoelement/tabpseudoelement.hpp"
#include "graphics/gui/luapseudoelement/contentpseudoelement.hpp"
#include "graphics/gui/luapseudoelement/itempseudoelement.hpp"
#include "graphics/gui/luapseudoelement/rowpseudoelement.hpp"
#include "graphics/gui/luaelement.hpp"
#include "graphics/widgetbuilder.hpp"
#include "graphics/display.hpp"
#include "tools/ctvalidators.hpp"
#include "tools/utility.hpp"
#include "log.hpp"
#include <tinyxml2.h>
#include <SFGUI/Widgets.hpp>
#include <exception>
#include <string>
#include <vector>

namespace BlueBear {
  namespace Graphics {
    namespace GUI {

      int LuaDesktopFunctions::lua_addXMLFromPath( lua_State* L ) {
        VERIFY_STRING( "LuaDesktopFunctions::lua_addXMLFromPath", "add_from_path" );
        Display::MainGameState* self = ( Display::MainGameState* )lua_touserdata( L, lua_upvalueindex( 1 ) );

        WidgetBuilder widgetBuilder( self->getImageCache() );

        try {
          std::vector< std::shared_ptr< sfg::Widget > > widgets = widgetBuilder.getWidgets( lua_tostring( L, -1 ) );

          for( auto widget : widgets ) {
            self->gui.desktop.Add( widget );
          }
        } catch( std::exception& e ) {
          Log::getInstance().error( "LuaDesktopFunctions::lua_addXMLFromPath", "Failed to add widget XML: " + std::string( e.what() ) );
        }

        return 0;
      }

      int LuaDesktopFunctions::lua_add( lua_State* L ) {
        Display::MainGameState* self = ( Display::MainGameState* )lua_touserdata( L, lua_upvalueindex( 1 ) );

        if( lua_isstring( L, -1 ) ) {
          WidgetBuilder widgetBuilder( self->getImageCache() );

          try {
            self->gui.desktop.Add( widgetBuilder.getWidgetFromXML( lua_tostring( L, -1 ) ) );
          } catch( std::exception& e ) {
            Log::getInstance().error( "LuaDesktopFunctions::lua_add", "Failed to add widget XML: " + std::string( e.what() ) );
          }

        } else {
          // Will throw exception if this does not match
          LuaElement* argument = *( ( LuaElement** ) luaL_checkudata( L, 1, "bluebear_widget" ) );
          self->gui.desktop.Add( argument->widget );
        }

        return 0;
      }

      int LuaDesktopFunctions::lua_createWidget( lua_State* L ) {
        VERIFY_STRING( "LuaDesktopFunctions::lua_createWidget", "create" );
        Display::MainGameState* self = ( Display::MainGameState* )lua_touserdata( L, lua_upvalueindex( 1 ) );

        tinyxml2::XMLDocument document;
        document.Parse( lua_tostring( L, -1 ) );

        if( document.ErrorID() ) {
          Log::getInstance().error( "LuaDesktopFunctions::lua_createWidget", "Could not parse XML string!" );
          return 0;
        }

        tinyxml2::XMLElement* element = document.RootElement();
        if( !element ) {
          Log::getInstance().error( "LuaDesktopFunctions::lua_createWidget", "No root element!" );
          return 0;
        }

        switch( Tools::Utility::hash( element->Name() ) ) {
          case Tools::Utility::hash( "tab" ):
            return TabPseudoElement::create( L, *self, element ); // userdata
          case Tools::Utility::hash( "content" ):
            return ContentPseudoElement::create( L, *self, element ); // userdata
          case Tools::Utility::hash( "page" ):
            return PagePseudoElement::create( L, *self, element ); // userdata
          case Tools::Utility::hash( "item" ):
            return ItemPseudoElement::create( L, *self, element ); // userdata
          case Tools::Utility::hash( "row" ):
            return RowPseudoElement::create( L, *self, element ); // userdata
          default:
            try {
              WidgetBuilder widgetBuilder( self->getImageCache() );

              LuaElement::getUserdataFromWidget( L, widgetBuilder.getWidgetFromElementDirect( element ) ); // userdata
              return 1;
            } catch( std::exception& e ) {
              Log::getInstance().error( "LuaDesktopFunctions::lua_createWidget", "Failed to add widget XML: " + std::string( e.what() ) );
              return 0;
            }
        }

        return 0;
      }

      int LuaDesktopFunctions::lua_removeWidget( lua_State* L ) {
        Display::MainGameState* self = ( Display::MainGameState* )lua_touserdata( L, lua_upvalueindex( 1 ) );

        LuaElement* argument = *( ( LuaElement** ) luaL_checkudata( L, 1, "bluebear_widget" ) );

        argument->widget->Show( false );
        self->gui.desktop.Remove( argument->widget );

        return 0;
      }

      int LuaDesktopFunctions::lua_findById( lua_State* L ) {
        VERIFY_STRING( "LuaDesktopFunctions::lua_findById", "find_by_id" );

        std::string selector( lua_tostring( L, -1 ) );

        if( std::shared_ptr< sfg::Widget > widget = LuaElement::getWidgetById( nullptr, selector ) ) {
          LuaElement::getUserdataFromWidget( L, widget ); // userdata "selector"
          return 1;
        } else {
          std::string error = std::string( "Could not find widget with ID " ) + selector;
          return luaL_error( L, error.c_str() );
        }

        return 0;
      }

      int LuaDesktopFunctions::lua_findByClass( lua_State* L ) {
        VERIFY_STRING( "LuaDesktopFunctions::lua_findByClass", "find_by_id" );

        std::string selector( lua_tostring( L, -1 ) );
        std::vector< std::shared_ptr< sfg::Widget > > widgets = LuaElement::getWidgetsByClass( nullptr, selector );

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

        return 0;
      }

    }
  }
}
