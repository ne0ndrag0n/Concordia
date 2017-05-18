#include "graphics/gui/luapseudoelement/pagepseudoelement.hpp"
#include "graphics/gui/luapseudoelement/nbbinpseudoelement.hpp"
#include "graphics/gui/luapseudoelement/tabpseudoelement.hpp"
#include "graphics/gui/luapseudoelement/contentpseudoelement.hpp"
#include "graphics/gui/luaelement.hpp"
#include "scripting/luakit/gchelper.hpp"
#include "tools/ctvalidators.hpp"
#include "tools/utility.hpp"
#include "eventmanager.hpp"
#include "log.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace GUI {

      PagePseudoElement::PagePseudoElement( std::shared_ptr< sfg::Notebook > subject, unsigned int pageNumber, Display::MainGameState& displayState )
        : subject( subject ), eventManager( displayState.instance.eventManager ), pageNumber( pageNumber ), displayState( displayState ) {
          listen();
        }

      PagePseudoElement::~PagePseudoElement() {
        deafen();
      }

      /**
       *
       * STACK ARGS: userdata
       * (Stack is unmodified after call)
       */
      void PagePseudoElement::setMetatable( lua_State* L ) {
        if( luaL_newmetatable( L, "bluebear_page_pseudo_element" ) ) { // metatable userdata
          luaL_Reg tableFuncs[] = {
            { "add", PagePseudoElement::lua_add },
            { "remove", PagePseudoElement::lua_removeWidget },
            { "get_name", PagePseudoElement::lua_getName },
            { "find_pseudo", PagePseudoElement::lua_findElement },
            { "find_by_id", PagePseudoElement::lua_findById },
            { "find_by_class", PagePseudoElement::lua_findByClass },
            { "get_property", PagePseudoElement::lua_getProperty },
            { "set_property", PagePseudoElement::lua_setProperty },
            { "get_content", PagePseudoElement::lua_getContent },
            { "set_content", PagePseudoElement::lua_setContent },
            { "__gc", PagePseudoElement::lua_gc },
            { NULL, NULL }
          };

          luaL_setfuncs( L, tableFuncs, 0 );

          lua_pushvalue( L, -1 ); // metatable metatable userdata

          lua_setfield( L, -2, "__index" ); // metatable userdata
        }

        lua_setmetatable( L, -2 ); // userdata
      }

      std::string PagePseudoElement::getName() {
        return "page";
      }

      void PagePseudoElement::onItemAdded( void* notebook, int changed ) {
        if( subject.get() == notebook && pageNumber >= changed ) {
          ++pageNumber;
        }
      }

      void PagePseudoElement::onItemRemoved( void* notebook, int changed ) {
        if( subject.get() == notebook && pageNumber > changed ) {
          --pageNumber;
        }
      }

      void PagePseudoElement::listen() {
        eventManager->ITEM_ADDED.listen( this, std::bind( &PagePseudoElement::onItemAdded, this, std::placeholders::_1, std::placeholders::_2 ) );
        eventManager->ITEM_REMOVED.listen( this, std::bind( &PagePseudoElement::onItemRemoved, this, std::placeholders::_1, std::placeholders::_2 ) );
      }

      void PagePseudoElement::deafen() {
        eventManager->ITEM_ADDED.stopListening( this );
        eventManager->ITEM_REMOVED.stopListening( this );
      }

      void PagePseudoElement::removeFromNotebook( std::shared_ptr< sfg::Widget > comparison ) {
        if( !subject || ( comparison != subject ) ) {
          Log::getInstance().warn( "PagePseudoElement::removeFromNotebook", "This <page> is not attached to this Notebook widget!" );
          return;
        }

        subject->RemovePage( pageNumber );
        eventManager->ITEM_REMOVED.trigger( subject.get(), pageNumber );

        pageNumber = -1;
        subject = nullptr;
      }

      /**
       *
       * STACK ARGS: none
       * Stack is unmodified after call
       */
      void PagePseudoElement::processXMLPseudoElement( lua_State* L, tinyxml2::XMLElement* child ) {
        // <tab> pseudo-element: screen, create
        // <content> pseudo-element: screen, create
        // Other elements - warn and ignore
        std::string tag( child->Name() );

        switch( Tools::Utility::hash( tag.c_str() ) ) {
          case Tools::Utility::hash( "tab" ): {
            if( !this->stagedTabElement ) {
              if( TabPseudoElement::create( L, displayState, child ) ) { // tabuserdata
                this->setStagedTabElement( L, *( ( NBBinPseudoElement** ) lua_topointer( L, -1 ) ) );

                lua_pop( L, 1 ); // EMPTY
              } // EMPTY
            } else {
              Log::getInstance().warn( "PagePseudoElement::processXMLPseudoElement", "Cannot add more than one <tab> child pseudo-element to <page> pseudo-element." );
            }
            break;
          }
          case Tools::Utility::hash( "content" ): {
            if( !this->stagedContentElement ) {
              if( ContentPseudoElement::create( L, displayState, child ) ) { // tabuserdata
                this->setStagedContentElement( L, *( ( NBBinPseudoElement** ) lua_topointer( L, -1 ) ) );

                lua_pop( L, 1 ); // EMPTY
              } // EMPTY
            } else {
              Log::getInstance().warn( "PagePseudoElement::processXMLPseudoElement", "Cannot add more than one <content> child pseudo-element to <page> pseudo-element." );
            }
            break;
          }
          default:
            Log::getInstance().warn( "PagePseudoElement::processXMLPseudoElement", "Cannot add element of type " + tag + " to <page> pseudo-element; ignoring." );
        }

      }

      /**
       *
       * STACK ARGS: none
       * Returns: userdata
       */
      int PagePseudoElement::create( lua_State* L, Display::MainGameState& displayState, tinyxml2::XMLElement* element ) {
        PagePseudoElement** userData = ( PagePseudoElement** ) lua_newuserdata( L, sizeof( PagePseudoElement* ) ); // userdata
        *userData = new PagePseudoElement( nullptr, 0, displayState );
        PagePseudoElement* self = *userData;
        self->setMetatable( L );

        for( tinyxml2::XMLElement* child = element->FirstChildElement(); child != NULL; child = child->NextSiblingElement() ) {
          self->processXMLPseudoElement( L, child );
        }

        return 1;
      }

      /**
       *
       * STACK ARGS: userdata if stagedTabElement is not null
       * Stack is unmodified after call
       */
      void PagePseudoElement::setStagedTabElement( lua_State* L, NBBinPseudoElement* stagedTabElement ) {
        if( this->stagedTabElement ) {
          Scripting::LuaKit::GcHelper::untrack(
            L,
            Tools::Utility::pointerToString( this ) + ":" + Tools::Utility::pointerToString( this->stagedTabElement )
          );
        }

        if( this->stagedTabElement = stagedTabElement ) {
          Scripting::LuaKit::GcHelper::track(
            L,
            Tools::Utility::pointerToString( this ) + ":" + Tools::Utility::pointerToString( this->stagedTabElement )
          );
        }
      }

      /**
       *
       * STACK ARGS: userdata if stagedContentElement is not null
       * Stack is unmodified after call
       */
      void PagePseudoElement::setStagedContentElement( lua_State* L, NBBinPseudoElement* stagedContentElement ) {
        if( this->stagedContentElement ) {
          Scripting::LuaKit::GcHelper::untrack(
            L,
            Tools::Utility::pointerToString( this ) + ":" + Tools::Utility::pointerToString( this->stagedContentElement )
          );
        }

        if( this->stagedContentElement = stagedContentElement ) {
          Scripting::LuaKit::GcHelper::track(
            L,
            Tools::Utility::pointerToString( this ) + ":" + Tools::Utility::pointerToString( this->stagedContentElement )
          );
        }
      }

      bool PagePseudoElement::setSubject( lua_State* L, std::shared_ptr< sfg::Notebook > notebook, int index ) {

        // Basic validation

        if( !notebook ) {
          Log::getInstance().error( "PagePseudoElement::setSubject", "std::shared_ptr< sfg::Notebook > was nullptr" );
          return false;
        }

        if( subject ) {
          Log::getInstance().warn( "PagePseudoElement::setSubject", "This <page> already belongs to a Notebook and cannot be added to another one." );
          return false;
        }

        if( ( !stagedTabElement || !stagedContentElement ) || ( !stagedTabElement->stagedWidget || !stagedContentElement->stagedWidget ) ) {
          Log::getInstance().warn( "PagePseudoElement::setSubject", "<page> pseudo-element cannot be added to a Notebook without both a <tab> and a <content> pseudo-element each containing one widget." );
          return false;
        }

        int newPageNumber = notebook->InsertPage( stagedContentElement->stagedWidget, stagedTabElement->stagedWidget, index );
        eventManager->ITEM_ADDED.trigger( notebook.get(), newPageNumber );

        subject = notebook;
        pageNumber = newPageNumber;

        stagedTabElement->setSubject( subject, pageNumber );
        stagedContentElement->setSubject( subject, pageNumber );

        this->setStagedTabElement( L, nullptr );
        this->setStagedContentElement( L, nullptr );

        return true;
      }

      /**
       * Use given XML to determine what should be added. One XML item at a time.
       */
      void PagePseudoElement::setStagedChild( lua_State* L, const std::string& xml ) {
        tinyxml2::XMLDocument document;
        document.Parse( xml.c_str() );

        if( document.ErrorID() ) {
          Log::getInstance().error( "PagePseudoElement::setStagedChild", "Could not parse XML string!" );
          return;
        }

        for( tinyxml2::XMLElement* child = document.RootElement(); child != NULL; child = child->NextSiblingElement() ) {
          this->processXMLPseudoElement( L, child );
        }
      }

      int PagePseudoElement::lua_add( lua_State* L ) {
        PagePseudoElement* self = *( ( PagePseudoElement** ) luaL_checkudata( L, 1, "bluebear_page_pseudo_element" ) );

        if( self->subject ) {
          Log::getInstance().warn( "PagePseudoElement::lua_add", "Cannot add to <page> pseudo-element when attached to an existing Notebook." );
          return 0;
        }

        // Determine if type is a string, bluebear_nbb_pseudo_element, or bluebear_content_pseudo_element (possibly may combine the last two)
        if( lua_isstring( L, -1 ) ) {
          self->setStagedChild( L, lua_tostring( L, -1 ) );
          return 0;
        }

        if( NBBinPseudoElement** udata = ( NBBinPseudoElement** ) luaL_testudata( L, 2, "bluebear_nbb_pseudo_element" ) ) {

          if( ( *udata )->getName() == "tab" ) {
            if( !self->stagedTabElement ) {
              self->setStagedTabElement( L, *udata );
            } else {
              Log::getInstance().warn( "PagePseudoElement::lua_add", "Cannot add more than one <tab> pseudo-element to <page> pseudo-element." );
            }
          }

          if( ( *udata )->getName() == "content" ) {
            if( !self->stagedContentElement ) {
              self->setStagedContentElement( L, *udata );
            } else {
              Log::getInstance().warn( "PagePseudoElement::lua_add", "Cannot add more than one <content> pseudo-element to <page> pseudo-element." );
            }
          }

          return 0;
        }

        Log::getInstance().warn( "PagePseudoElement::lua_add", "Invalid argument passed to add()" );
        return 0;
      }

      int PagePseudoElement::lua_removeWidget( lua_State* L ) {
        PagePseudoElement* self = *( ( PagePseudoElement** ) luaL_checkudata( L, 1, "bluebear_page_pseudo_element" ) );

        if( self->subject ) {
          Log::getInstance().warn( "PagePseudoElement::lua_removeWidget", "Cannot remove elements or pseudo-elements from <page> pseudo-element when attached to an existing Notebook." );
          return 0;
        }

        // Unstaged <page>
        // Accept only <tab> or <content> pseudo-elements directly
        if( NBBinPseudoElement** udata = ( NBBinPseudoElement** ) luaL_testudata( L, 2, "bluebear_nbb_pseudo_element" ) ) {

          if( ( *udata )->getName() == "tab" ) {
            if( self->stagedTabElement == *udata ) {
              self->setStagedTabElement( L, nullptr );
            }
          }

          if( ( *udata )->getName() == "content" ) {
            if( self->stagedContentElement == *udata ) {
              self->setStagedContentElement( L, nullptr );
            }
          }

          return 0;
        }

        return 0;
      }

      int PagePseudoElement::lua_getName( lua_State* L ) {
        PagePseudoElement* self = *( ( PagePseudoElement** ) luaL_checkudata( L, 1, "bluebear_page_pseudo_element" ) );

        lua_pushstring( L, self->getName().c_str() );

        return 1;
      }

      /**
       *
       * STACK ARGS: none
       * Returns: userdata, or nothing if a userdata cannot be created.
       */
      bool PagePseudoElement::findElement( lua_State* L, const std::string& tag ) {
        switch( Tools::Utility::hash( tag.c_str() ) ) {
          case Tools::Utility::hash( "tab" ): {
            NBBinPseudoElement** userData = ( NBBinPseudoElement** ) lua_newuserdata( L, sizeof( NBBinPseudoElement* ) ); // userdata
            *userData = new TabPseudoElement( subject, pageNumber, displayState );
            ( *userData )->setMetatable( L );

            return true;
          }
          case Tools::Utility::hash( "content" ): {
            NBBinPseudoElement** userData = ( NBBinPseudoElement** ) lua_newuserdata( L, sizeof( NBBinPseudoElement* ) ); // userdata
            *userData = new ContentPseudoElement( subject, pageNumber, displayState );
            ( *userData )->setMetatable( L );

            return true;
          }
          default:
            Log::getInstance().warn( "PagePseudoElement::findElement", "<page> pseduo-element only contains pseudo-elements of type <tab> or <content>." );
        }

        return false;
      }

      /**
       *
       * STACK ARGS: none
       * Returns: userdata, or nothing if a userdata cannot be created.
       */
      int PagePseudoElement::findElementStaged( lua_State* L, const std::string& tag ) {
        switch( Tools::Utility::hash( tag.c_str() ) ) {
          case Tools::Utility::hash( "tab" ): {
            if( this->stagedTabElement ) {
              Scripting::LuaKit::GcHelper::getTracked(
                L,
                Tools::Utility::pointerToString( this ) + ":" + Tools::Utility::pointerToString( this->stagedTabElement )
              ); // userdata/nil

              return lua_isnil( L, -1 ) ? 0 : 1;
            } else {
              Log::getInstance().debug( "PagePseudoElement::findElementStaged", "No <tab> element present in this <page>." );
            }

            break;
          }
          case Tools::Utility::hash( "content" ): {
            if( this->stagedContentElement ) {
              Scripting::LuaKit::GcHelper::getTracked(
                L,
                Tools::Utility::pointerToString( this ) + ":" + Tools::Utility::pointerToString( this->stagedContentElement )
              ); // userdata/nil

              return lua_isnil( L, -1 ) ? 0 : 1;
            } else {
              Log::getInstance().debug( "PagePseudoElement::findElementStaged", "No <content> element present in this <page>." );
            }

            break;
          }
          default:
            Log::getInstance().warn( "PagePseudoElement::findElementStaged", "<page> pseduo-element only contains pseudo-elements of type <tab> or <content>." );
        }

        return 0;
      }

      /**
       *
       * STACK ARGS: none
       * Returns: table, or none
       */
      int PagePseudoElement::getElementsByClass( lua_State* L, const std::string& classID ) {
        std::unique_ptr< NBBinPseudoElement > holdingPtr = nullptr;
        int stack = 0;

        if( NBBinPseudoElement* content = getPseudoElement< ContentPseudoElement >( holdingPtr, stagedContentElement ) ) {
          stack = content->getElementsByClass( L, classID ); // table/none
        }

        if( NBBinPseudoElement* tab = getPseudoElement< TabPseudoElement >( holdingPtr, stagedTabElement ) ) {
          if( stack ) {
            // Coalesce tables
            if( tab->getElementsByClass( L, classID ) ) { // table table
              int firstTableIndex = lua_rawlen( L, -2 ) + 1;
              int secondTableMax = lua_rawlen( L, -1 ) + 1;

              for( int i = 0; i != secondTableMax; i++ ) {
                lua_rawgeti( L, -1, i ); // item table table
                lua_rawseti( L, -3, firstTableIndex++ ); // table table
              }

              lua_pop( L, 1 ); // table
            }
          } else {
            // No table created previously, so just go with the stack
            stack = tab->getElementsByClass( L, classID ); // table/none
          }
        }

        return stack;
      }

      /**
       *
       * STACK ARGS: none
       * Returns: userdata, or none
       */
      int PagePseudoElement::getElementById( lua_State* L, const std::string& id ) {

        std::unique_ptr< NBBinPseudoElement > holdingPtr = nullptr;

        if( NBBinPseudoElement* content = getPseudoElement< ContentPseudoElement >( holdingPtr, stagedContentElement ) ) {
          if( int stack = content->getElementById( L, id ) ) { // userdata/none
            return stack;
          }
        }

        if( NBBinPseudoElement* tab = getPseudoElement< TabPseudoElement >( holdingPtr, stagedTabElement ) ) {
          if( int stack = tab->getElementById( L, id ) ) { // userdata/none
            return stack;
          }
        }

        return 0;
      }

      int PagePseudoElement::lua_findElement( lua_State* L ) {
        VERIFY_STRING( "PagePseudoElement::lua_findElement", "find_pseudo" );

        PagePseudoElement* self = *( ( PagePseudoElement** ) luaL_checkudata( L, 1, "bluebear_page_pseudo_element" ) );

        // <page> <tab /> <content /> </page>
        if( self->subject ) {
          return self->findElement( L, lua_tostring( L, -1 ) ) ? 1 : 0; // userdata or none
        } else {
          return self->findElementStaged( L, lua_tostring( L, -1 ) ); // userdata or none
        }
      }

      int PagePseudoElement::lua_findById( lua_State* L ) {
        VERIFY_STRING( "PagePseudoElement::lua_findById", "find_by_id" );

        PagePseudoElement* self = *( ( PagePseudoElement** ) luaL_checkudata( L, 1, "bluebear_page_pseudo_element" ) );

        return self->getElementById( L, lua_tostring( L, -1 ) );
      }
      int PagePseudoElement::lua_findByClass( lua_State* L ) {
        VERIFY_STRING( "PagePseudoElement::lua_findByClass", "find_by_class" );

        PagePseudoElement* self = *( ( PagePseudoElement** ) luaL_checkudata( L, 1, "bluebear_page_pseudo_element" ) );

        return self->getElementsByClass( L, lua_tostring( L, -1 ) );
      }

      int PagePseudoElement::lua_getProperty( lua_State* L ) {
        Log::getInstance().warn( "PagePseudoElement::lua_getProperty", "<page> pseudo-element has no gettable properties." );
        return 0;
      }

      int PagePseudoElement::lua_setProperty( lua_State* L ) {
        Log::getInstance().warn( "PagePseudoElement::lua_setProperty", "<page> pseudo-element has no settable properties." );
        return 0;
      }

      int PagePseudoElement::lua_getContent( lua_State* L ) {
        Log::getInstance().warn( "PagePseudoElement::lua_getContent", "<page> pseudo-element has no direct content." );
        return 0;
      }

      int PagePseudoElement::lua_setContent( lua_State* L ) {
        Log::getInstance().warn( "PagePseudoElement::lua_setContent", "<page> pseudo-element has no direct content." );
        return 0;
      }

      int PagePseudoElement::lua_gc( lua_State* L ) {
        PagePseudoElement* widgetPtr = *( ( PagePseudoElement** ) luaL_checkudata( L, 1, "bluebear_page_pseudo_element" ) );

        widgetPtr->setStagedTabElement( L, nullptr );
        widgetPtr->setStagedContentElement( L, nullptr );

        delete widgetPtr;

        return 0;
      }

    }
  }
}
