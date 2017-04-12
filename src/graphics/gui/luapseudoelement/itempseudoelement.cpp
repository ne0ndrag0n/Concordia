#include "graphics/gui/luapseudoelement/itempseudoelement.hpp"
#include "tools/ctvalidators.hpp"
#include "log.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace GUI {

      ItemPseudoElement::ItemPseudoElement( std::shared_ptr< sfg::ComboBox > subject, int elementNumber ) : subject( subject ), elementNumber( elementNumber ) {}

      /**
       *
       * STACK ARGS: userdata
       * (Stack is unmodified after call)
       */
      void ItemPseudoElement::setMetatable( lua_State* L ) {
        if( luaL_newmetatable( L, "bluebear_item_pseudo_element" ) ) { // metatable userdata
          luaL_Reg tableFuncs[] = {
            { "add", ItemPseudoElement::lua_add },
            { "remove", ItemPseudoElement::lua_removeWidget },
            { "get_name", ItemPseudoElement::lua_getName },
            { "find_pseudo", ItemPseudoElement::lua_findElement },
            { "find_by_id", ItemPseudoElement::lua_findBy },
            { "find_by_class", ItemPseudoElement::lua_findBy },
            { "get_property", ItemPseudoElement::lua_property },
            { "set_property", ItemPseudoElement::lua_property },
            { "get_content", ItemPseudoElement::lua_getContent },
            { "set_content", ItemPseudoElement::lua_setContent },
            { "__gc", ItemPseudoElement::lua_gc },
            { NULL, NULL }
          };

          luaL_setfuncs( L, tableFuncs, 0 );

          lua_pushvalue( L, -1 ); // metatable metatable userdata

          lua_setfield( L, -2, "__index" ); // metatable userdata
        }

        lua_setmetatable( L, -2 ); // userdata
      }

      std::string ItemPseudoElement::getName() {
        return "item";
      }

      void ItemPseudoElement::setItem( const std::string& content ) {
        if( subject ) {
          subject->ChangeItem( elementNumber, content );
        } else {
          stagedItem = content;
        }
      }

      std::string ItemPseudoElement::getItem() {
        return subject ? ( std::string ) subject->GetItem( elementNumber ) : stagedItem;
      }

      int ItemPseudoElement::lua_add( lua_State* L ) {
        Log::getInstance().warn( "ItemPseudoElement::lua_add", "Cannot add to <item> pseudo-element." );
        return 0;
      }

      int ItemPseudoElement::lua_removeWidget( lua_State* L ) {
        Log::getInstance().warn( "ItemPseudoElement::lua_removeWidget", "Cannot remove from <item> pseudo-element." );
        return 0;
      }

      int ItemPseudoElement::lua_getName( lua_State* L ) {
        ItemPseudoElement* self = *( ( ItemPseudoElement** ) luaL_checkudata( L, 1, "bluebear_item_pseudo_element" ) );

        lua_pushstring( L, self->getName().c_str() );

        return 1;
      }

      int ItemPseudoElement::lua_findElement( lua_State* L ) {
        Log::getInstance().warn( "ItemPseudoElement::lua_findElement", "<item> pseudo-element has no pseudo-element children." );
        return 0;
      }

      int ItemPseudoElement::lua_findBy( lua_State* L ) {
        Log::getInstance().warn( "ItemPseudoElement::lua_findBy", "<item> pseudo-element has no widget children." );
        return 0;
      }

      int ItemPseudoElement::lua_property( lua_State* L ) {
        Log::getInstance().warn( "ItemPseudoElement::lua_property", "<item> pseudo-element has no properties." );
        return 0;
      }

      int ItemPseudoElement::lua_getContent( lua_State* L ) {
        ItemPseudoElement* self = *( ( ItemPseudoElement** ) luaL_checkudata( L, 1, "bluebear_item_pseudo_element" ) );

        lua_pushstring( L, self->getItem().c_str() ); // "item"

        return 1;
      }

      int ItemPseudoElement::lua_setContent( lua_State* L ) {
        VERIFY_STRING( "ItemPseudoElement::lua_setContent", "set_content" );
        ItemPseudoElement* self = *( ( ItemPseudoElement** ) luaL_checkudata( L, 1, "bluebear_item_pseudo_element" ) );

        self->setItem( lua_tostring( L, -1 ) );

        return 0;
      }

      int ItemPseudoElement::lua_gc( lua_State* L ) {
        ItemPseudoElement* self = *( ( ItemPseudoElement** ) luaL_checkudata( L, 1, "bluebear_item_pseudo_element" ) );

        delete self;

        return 0;
      }

    }
  }
}
