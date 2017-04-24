#include "graphics/gui/luapseudoelement/rowpseudoelement.hpp"
#include "graphics/gui/luaelement.hpp"
#include "graphics/widgetbuilder.hpp"
#include "tools/ctvalidators.hpp"
#include "tools/utility.hpp"
#include "log.hpp"
#include <algorithm>
#include <set>
#include <tinyxml2.h>

namespace BlueBear {
  namespace Graphics {
    namespace GUI {

      RowPseudoElement::RowPseudoElement( std::shared_ptr< sfg::Table > subject, int rowNumber, Display::MainGameState& displayState ) :
        subject( subject ), rowNumber( rowNumber ), displayState( displayState ) {}


      /**
       *
       * STACK ARGS: userdata
       * (Stack is unmodified after call)
       */
      void RowPseudoElement::setMetatable( lua_State* L ) {
        if( luaL_newmetatable( L, "bluebear_row_pseudo_element" ) ) { // metatable userdata
          luaL_Reg tableFuncs[] = {
            { "add", RowPseudoElement::lua_add },
            /*
            { "remove", RowPseudoElement::lua_removeWidget },
            */
            { "get_name", RowPseudoElement::lua_getName },
            { "find_pseudo", RowPseudoElement::lua_findElement },
            { "find_by_id", RowPseudoElement::lua_findById },
            { "find_by_class", RowPseudoElement::lua_findByClass },
            /*
            { "get_property", RowPseudoElement::lua_property },
            { "set_property", RowPseudoElement::lua_property },
            { "get_content", RowPseudoElement::lua_getContent },
            { "set_content", RowPseudoElement::lua_setContent },
            */
            { "__gc", RowPseudoElement::lua_gc },
            { NULL, NULL }
          };

          luaL_setfuncs( L, tableFuncs, 0 );

          lua_pushvalue( L, -1 ); // metatable metatable userdata

          lua_setfield( L, -2, "__index" ); // metatable userdata
        }

        lua_setmetatable( L, -2 ); // userdata
      }

      std::string RowPseudoElement::getName() {
        return "row";
      }

      /**
       * @static
       */
      int RowPseudoElement::getRowCount( std::shared_ptr< sfg::Table > table ) {
        std::set< sf::Uint32 > counts;

        std::list< sfg::priv::TableCell > tableCellList = table->m_cells;
        std::for_each( tableCellList.begin(), tableCellList.end(), [ & ]( sfg::priv::TableCell& cell ) {
          // Top should contain row number
          // Set should never add two of the same
          counts.insert( cell.rect.top );
        } );

        return counts.size();
      }

      std::vector< std::shared_ptr< sfg::Widget > > RowPseudoElement::getWidgetsForRow() {
        std::vector< std::shared_ptr< sfg::Widget > > widgets;

        if( subject ) {
          // Use this magic using the exposed properties in ne0ndrag0n/SFGUI
          // Copy the table cell list
          std::list< sfg::priv::TableCell > tableCellList = subject->m_cells;

          std::for_each( tableCellList.begin(), tableCellList.end(), [ & ]( sfg::priv::TableCell& cell ) {
            // Top should contain the row number
            if( cell.rect.top == rowNumber ) {
              widgets.push_back( cell.child );
            }
          } );
        } else {
          // Extract all the stagedWidgets
          for( RowPseudoElement::WidgetStaging& staging : stagedWidgets ) {
            widgets.push_back( staging.widget );
          }
        }

        return widgets;
      }

      /**
       *
       * STACK ARGS: none
       * Returns: userdata or none
       */
      int RowPseudoElement::getItemById( lua_State* L, const std::string& id ) {
        std::vector< std::shared_ptr< sfg::Widget > > widgets = getWidgetsForRow();

        for( std::shared_ptr< sfg::Widget > widget : widgets ) {

          // Is this the widget we need?
          if( widget->GetId() == id ) {
            LuaElement::getUserdataFromWidget( L, widget ); // userdata
            return 1;
          }

          // FIXME: ANOTHER sfgui bug. The sibling widgets "foo" and "bar", existing in two different rows in the table, will be reported as
          // "bar" being the parent of "foo". This breaks the specification of ConcordiaME but is not necessarily a deal-breaking bug.
          //
          // A potential workaround can be made by checking the parent chain of the discovered widget. If "widget" is not anywhere in the parent
          // chain, then this result is not valid.
          //
          // Workaround is implemented below. TODO check this in other areas where container based items are used. Bug reports might come in.

          // Does this widget tree contain the widget we need?
          if( Tools::Utility::widgetIsContainer( widget ) ) {

            std::shared_ptr< sfg::Widget > found = widget->GetWidgetById( id );
            if( found && Tools::Utility::isActualParent( found, std::static_pointer_cast< sfg::Container >( widget ) ) ) {
              LuaElement::getUserdataFromWidget( L, found ); // userdata
              return 1;
            }

          }

        }

        Log::getInstance().warn( "RowPseudoElement::getElementById", "No elements of ID " + id + " found in this pseudo-element." );
        return 0;
      }

      /**
       *
       * STACK ARGS: none
       * Returns: array, single userdata, or none
       */
      int RowPseudoElement::getItemsByClass( lua_State* L, const std::string clss ) {
        std::vector< std::shared_ptr< sfg::Widget > > widgets = getWidgetsForRow();
        std::vector< std::shared_ptr< sfg::Widget > > results;

        // Pare this list down from widgets into results
        for( std::shared_ptr< sfg::Widget > widget : widgets ) {

          if( widget->GetClass() == clss ) {
            results.push_back( widget );
          }

          if( Tools::Utility::widgetIsContainer( widget ) ) {

             std::vector< std::shared_ptr< sfg::Widget > > all = widget->GetWidgetsByClass( clss );
             for( std::shared_ptr< sfg::Widget > nestedWidget : all ) {
               if( Tools::Utility::isActualParent( nestedWidget, std::static_pointer_cast< sfg::Container >( widget ) ) ) {
                 results.push_back( nestedWidget );
               }
             }
          }

        }

        int resultSize = results.size();
        if( resultSize == 1 ) {
          // Single item, return direct
          LuaElement::getUserdataFromWidget( L, results[ 0 ] ); // userdata
          return 1;
        } else if( resultSize ) {
          // Multiple items, return array

          lua_createtable( L, resultSize, 0 ); // table

          for( int i = 0; i != resultSize; i++ ) {
            LuaElement::getUserdataFromWidget( L, results[ i ] ); // userdata table
            lua_rawseti( L, -2, i + 1 ); // table
          }

          return 1;
        }

        Log::getInstance().warn( "RowPseudoElement::getItemsByClass", "No elements of class " + clss + " found in this pseudo-element." );
        return 0;
      }

      void RowPseudoElement::add( lua_State* L, LuaElement* element ) {

        // This is just a disgrace
        unsigned int colspan = 1;
        LuaElement::queryUnsignedAttribute( element->widget, "colspan", &colspan );
        unsigned int rowspan = 1;
        LuaElement::queryUnsignedAttribute( element->widget, "rowspan", &rowspan );
        float paddingX = 0.0f;
        LuaElement::queryFloatAttribute( element->widget, "padding_x", &paddingX );
        float paddingY = 0.0f;
        LuaElement::queryFloatAttribute( element->widget, "padding_y", &paddingY );
        bool expandX = true, expandY = true;
        bool fillX = true, fillY = true;

        LuaElement::queryBoolAttribute( element->widget, "expand_x", &expandX );
        LuaElement::queryBoolAttribute( element->widget, "expand_y", &expandX );
        LuaElement::queryBoolAttribute( element->widget, "fill_x", &fillX );
        LuaElement::queryBoolAttribute( element->widget, "fill_y", &fillY );

        int packX = 0, packY = 0;
        if( expandX ) { packX |= sfg::Table::EXPAND; }
        if( fillX ) { packX |= sfg::Table::FILL; }
        if( expandY ) { packY |= sfg::Table::EXPAND; }
        if( fillY ) { packY |= sfg::Table::FILL; }

        add(
          RowPseudoElement::WidgetStaging{
            colspan, rowspan, paddingX, paddingY, packX, packY, element->widget
          }
        );
      }

      void RowPseudoElement::add( lua_State* L, const std::string& xmlString ) {
        // Manually parse this out - We'll be adding a widget but it has additional properties that need to be passed to the shared_ptr overload
        tinyxml2::XMLDocument document;

        for( tinyxml2::XMLElement* child = Tools::Utility::getRootNode( document, xmlString ); child != NULL; child = child->NextSiblingElement() ) {
          try {
            WidgetBuilder widgetBuilder( displayState.instance.eventManager, displayState.getImageCache() );
            unsigned int colspan = 1;
            child->QueryUnsignedAttribute( "colspan", &colspan );

            unsigned int rowspan = 1;
            child->QueryUnsignedAttribute( "rowspan", &rowspan );

            float paddingX = 0.0f;
            child->QueryFloatAttribute( "padding_x", &paddingX );

            float paddingY = 0.0f;
            child->QueryFloatAttribute( "padding_y", &paddingY );

            bool expandX = true, expandY = true;
            bool fillX = true, fillY = true;

            child->QueryBoolAttribute( "expand_x", &expandX );
            child->QueryBoolAttribute( "expand_y", &expandY );
            child->QueryBoolAttribute( "fill_x", &fillX );
            child->QueryBoolAttribute( "fill_y", &fillY );

            int packX = 0, packY = 0;
            if( expandX ) { packX |= sfg::Table::EXPAND; }
            if( fillX ) { packX |= sfg::Table::FILL; }
            if( expandY ) { packY |= sfg::Table::EXPAND; }
            if( fillY ) { packY |= sfg::Table::FILL; }

            std::shared_ptr< sfg::Widget > widget = widgetBuilder.getWidgetFromElementDirect( child );
            if( !subject ) {
              LuaElement::setCustomAttribute( widget, "colspan", std::to_string( colspan ) );
              LuaElement::setCustomAttribute( widget, "rowspan", std::to_string( rowspan ) );
              LuaElement::setCustomAttribute( widget, "padding_x", std::to_string( paddingX ) );
              LuaElement::setCustomAttribute( widget, "padding_y", std::to_string( paddingY ) );
              LuaElement::setCustomAttribute( widget, "expand_x", std::to_string( expandX ) );
              LuaElement::setCustomAttribute( widget, "expand_y", std::to_string( expandY ) );
              LuaElement::setCustomAttribute( widget, "fill_x", std::to_string( fillX ) );
              LuaElement::setCustomAttribute( widget, "fill_y", std::to_string( fillY ) );
            }

            add(
              RowPseudoElement::WidgetStaging{
                colspan,
                rowspan,
                paddingX,
                paddingY,
                packX,
                packY,
                widget
              }
            );

          } catch( std::exception& e ) {
            Log::getInstance().error( "RowPseudoElement::add", "Failed to add widget XML: " + std::string( e.what() ) );
          }
        }
      }

      void RowPseudoElement::add( RowPseudoElement::WidgetStaging staging ) {
        if( subject ) {
          subject->Attach(
            staging.widget,
            sf::Rect< sf::Uint32 >( getLatestColumn() + 1, rowNumber, staging.colspan, staging.rowspan ),
            staging.packX,
            staging.packY,
            sf::Vector2f( staging.paddingX, staging.paddingY )
          );
        } else {
          stagedWidgets.push_back( staging );
        }
      }

      int RowPseudoElement::getLatestColumn() {
        if( subject ) {
          int largestColumn = -1;

          std::list< sfg::priv::TableCell > tableCellList = subject->m_cells;
          std::for_each( tableCellList.begin(), tableCellList.end(), [ & ]( sfg::priv::TableCell& cell ) {
            if( cell.rect.top == rowNumber && (int) cell.rect.left > largestColumn ) {
              largestColumn = cell.rect.left;
            }
          } );

          return largestColumn;
        } else {
          return stagedWidgets.size();
        }
      }

      int RowPseudoElement::lua_add( lua_State* L ) {
        RowPseudoElement* self = *( ( RowPseudoElement** ) luaL_checkudata( L, 1, "bluebear_row_pseudo_element" ) );

        if( lua_isstring( L, -1 ) ) {
          self->add( L, lua_tostring( L, -1 ) );
        } else {
          LuaElement* element = *( ( LuaElement** ) luaL_checkudata( L, 2, "bluebear_widget" ) );
          self->add( L, element );
        }
      }

      int RowPseudoElement::lua_findById( lua_State* L ) {
        VERIFY_STRING( "RowPseudoElement::lua_findById", "find_by_id" );
        RowPseudoElement* self = *( ( RowPseudoElement** ) luaL_checkudata( L, 1, "bluebear_row_pseudo_element" ) );

        return self->getItemById( L, lua_tostring( L, -1 ) ); // userdata/none
      }

      int RowPseudoElement::lua_findByClass( lua_State* L ) {
        VERIFY_STRING( "RowPseudoElement::lua_findByClass", "find_by_class" );
        RowPseudoElement* self = *( ( RowPseudoElement** ) luaL_checkudata( L, 1, "bluebear_row_pseudo_element" ) );

        return self->getItemsByClass( L, lua_tostring( L, -1 ) ); // userdata/none
      }

      int RowPseudoElement::lua_getName( lua_State* L ) {
        RowPseudoElement* self = *( ( RowPseudoElement** ) luaL_checkudata( L, 1, "bluebear_row_pseudo_element" ) );

        lua_pushstring( L, self->getName().c_str() );

        return 1;
      }

      int RowPseudoElement::lua_findElement( lua_State* L ) {
        Log::getInstance().warn( "RowPseudoElement::lua_findElement", "<row> pseudo-element has no pseudo-element children." );
        return 0;
      }

      int RowPseudoElement::lua_findPseudo( lua_State* L ) {
        Log::getInstance().warn( "RowPseudoElement::lua_findPseudo", "<row> pseudo-element has no pseudo-element children." );
        return 0;
      }

      int RowPseudoElement::lua_gc( lua_State* L ) {
        // TODO cleanup of masterAttrMap

        RowPseudoElement* self = *( ( RowPseudoElement** ) luaL_checkudata( L, 1, "bluebear_row_pseudo_element" ) );

        delete self;

        return 0;
      }

    }
  }
}
