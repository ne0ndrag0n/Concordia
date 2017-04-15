#include "graphics/gui/luapseudoelement/rowpseudoelement.hpp"
#include "log.hpp"
#include <algorithm>

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
            { "find_by_id", RowPseudoElement::lua_findBy },
            /*
            { "find_by_class", RowPseudoElement::lua_findBy },
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

      std::vector< std::shared_ptr< sfg::Widget > > RowPseudoElement::getWidgetsForRow() {
        std::vector< std::shared_ptr< sfg::Widget > > widgets;

        // Use this magic using the exposed properties in ne0ndrag0n/SFGUI
        if( subject ) {
          // Copy the table cell list
          std::list< sfg::priv::TableCell > tableCellList = subject->m_cells;

          std::for_each( tableCellList.begin(), tableCellList.end(), [ & ]( sfg::priv::TableCell& cell ) {
            // Top should contain the row number
            if( cell.rect.top == rowNumber ) {
              widgets.push_back( cell.child );
            }
          } );
        }

        return widgets;
      }

      int RowPseudoElement::getItemById( const std::string& id ) {
        return 0;
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
