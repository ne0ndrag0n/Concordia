#include "graphics/gui/luapseudoelement/rowpseudoelement.hpp"
#include "graphics/gui/luaelement.hpp"
#include "tools/ctvalidators.hpp"
#include "tools/utility.hpp"
#include "log.hpp"
#include <algorithm>
#include <set>

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
            /*
            { "add", RowPseudoElement::lua_add },
            { "remove", RowPseudoElement::lua_removeWidget },
            */
            { "get_name", RowPseudoElement::lua_getName },
            /*
            { "find_pseudo", RowPseudoElement::lua_findElement },
            */
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
        if( subject ) {
          // Use this magic using the exposed properties in ne0ndrag0n/SFGUI
          std::vector< std::shared_ptr< sfg::Widget > > widgets;

          // Copy the table cell list
          std::list< sfg::priv::TableCell > tableCellList = subject->m_cells;

          std::for_each( tableCellList.begin(), tableCellList.end(), [ & ]( sfg::priv::TableCell& cell ) {
            // Top should contain the row number
            if( cell.rect.top == rowNumber ) {
              widgets.push_back( cell.child );
            }
          } );

          return widgets;
        } else {
          return stagedWidgets;
        }
      }

      /**
       *
       * STACK ARGS: none
       * Returns: userdata or none
       */
      int RowPseudoElement::getItemById( lua_State* L, const std::string& id ) {
        std::vector< std::shared_ptr< sfg::Widget > > widgets = getWidgetsForRow();

        if( int size = widgets.size() ) {
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
        }

        Log::getInstance().warn( "RowPseudoElement::getElementById", "No elements of ID " + id + " found in this pseudo-element." );
        return 0;
      }

      /**
       *
       * STACK ARGS: none
       * Returns: userdata or none
       */
      int RowPseudoElement::getItemsByClass( lua_State* L, const std::string clss ) {
        // TODO
        return 0;
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

      int RowPseudoElement::lua_gc( lua_State* L ) {
        RowPseudoElement* self = *( ( RowPseudoElement** ) luaL_checkudata( L, 1, "bluebear_row_pseudo_element" ) );

        delete self;

        return 0;
      }

    }
  }
}
