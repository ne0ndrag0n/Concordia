#include "graphics/gui/luapseudoelement/itempseudoelement.hpp"
#include "tools/ctvalidators.hpp"
#include "eventmanager.hpp"
#include "log.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace GUI {

      ItemPseudoElement::ItemPseudoElement( std::shared_ptr< sfg::ComboBox > subject, int elementNumber, Display::MainGameState& displayState )
        : subject( subject ), eventManager( displayState.instance.eventManager ), elementNumber( elementNumber ), displayState( displayState ) {
        listen();
      }

      ItemPseudoElement::~ItemPseudoElement() {
        deafen();
      }

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

      int ItemPseudoElement::create( lua_State* L, Display::MainGameState& displayState, tinyxml2::XMLElement* element ) {
        ItemPseudoElement** item = ( ItemPseudoElement** ) lua_newuserdata( L, sizeof( ItemPseudoElement* ) ); // userdata
        *item = new ItemPseudoElement( nullptr, -1, displayState );
        ( *item )->setMetatable( L );

        // Stage content
        if( const char* text = element->GetText() ) {
          ( *item )->stagedItem = text;
        }

        return 1;
      }

      void ItemPseudoElement::removeFromComboBox( std::shared_ptr< sfg::Widget > comparison ) {
        if( !subject || ( comparison != subject ) ) {
          Log::getInstance().warn( "ItemPseudoElement::removeFromComboBox", "This <item> is not attached to this ComboBox widget!" );
          return;
        }

        subject->RemoveItem( elementNumber );
        eventManager->ITEM_REMOVED.trigger( subject.get(), elementNumber );

        elementNumber = -1;

        subject = nullptr;
      }


      std::string ItemPseudoElement::getName() {
        return "item";
      }

      void ItemPseudoElement::listen() {
        eventManager->ITEM_ADDED.listen( this, std::bind( &ItemPseudoElement::onItemAdded, this, std::placeholders::_1, std::placeholders::_2 ) );
        eventManager->ITEM_REMOVED.listen( this, std::bind( &ItemPseudoElement::onItemRemoved, this, std::placeholders::_1, std::placeholders::_2 ) );
      }

      void ItemPseudoElement::deafen() {
        eventManager->ITEM_ADDED.stopListening( this );
        eventManager->ITEM_REMOVED.stopListening( this );
      }

      void ItemPseudoElement::onItemAdded( void* item, int changed ) {
        if( subject.get() == item && elementNumber >= changed ) {
          ++elementNumber;
        }
      }

      void ItemPseudoElement::onItemRemoved( void* item, int changed ) {
        if( subject.get() == item && elementNumber > changed ) {
          --elementNumber;
        }
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

      /**
       * Note: turns out we didn't really need the bool in PagePseudoElement's setSubject
       */
      void ItemPseudoElement::setSubject( std::shared_ptr< sfg::ComboBox > comboBox, int index ) {

        if( !comboBox ) {
          Log::getInstance().error( "ItemPseudoElement::setSubject", "std::shared_ptr< sfg::ComboBox > was nullptr" );
          return;
        }

        if( subject ) {
          Log::getInstance().warn( "ItemPseudoElement::setSubject", "This <item> already belongs to a ComboBox and cannot be added to another one." );
          return;
        }

        subject = comboBox;

        int newElementNumber;

        if( index >= 0 && index <= subject->GetItemCount() - 1 ) {
          subject->InsertItem( index, stagedItem );
          newElementNumber = index;
        } else {
          subject->AppendItem( stagedItem );
          newElementNumber = subject->GetItemCount() - 1;
        }

        eventManager->ITEM_ADDED.trigger( subject.get(), newElementNumber );
        elementNumber = newElementNumber;

        stagedItem = "";
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
