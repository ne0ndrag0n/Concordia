#include "graphics/gui/luapseudoelement/pagepseudoelement.hpp"
#include "graphics/gui/luapseudoelement/nbbinpseudoelement.hpp"
#include "graphics/gui/luapseudoelement/tabpseudoelement.hpp"
#include "graphics/gui/luaelement.hpp"
#include "scripting/luakit/gchelper.hpp"
#include "tools/ctvalidators.hpp"
#include "tools/utility.hpp"
#include "log.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace GUI {

      PagePseudoElement::PagePseudoElement( std::shared_ptr< sfg::Notebook > subject, unsigned int pageNumber, Display::MainGameState& displayState )
        : subject( subject ), pageNumber( pageNumber ), displayState( displayState ), stagedTabElement( nullptr ) {}

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
            Log::getInstance().error( "PagePseudoElement::processXMLPseudoElement", "Unimplemented" );
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
          if( !self->stagedTabElement ) {
            self->setStagedTabElement( L, *udata );
          } else {
            Log::getInstance().warn( "PagePseudoElement::lua_add", "Cannot add more than one <tab> pseudo-element to <page> pseudo-element." );
          }
          return 0;
        }

        // TODO: content

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

          if( self->stagedTabElement == *udata ) {
            self->setStagedTabElement( L, nullptr );
          }

          return 0;
        }

        // TODO: content

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
          case Tools::Utility::hash( "tab" ):
            {
              NBBinPseudoElement** userData = ( NBBinPseudoElement** ) lua_newuserdata( L, sizeof( NBBinPseudoElement* ) ); // userdata
              *userData = new TabPseudoElement( subject, pageNumber, displayState );
              ( *userData )->setMetatable( L );
            }
            return true;
          case Tools::Utility::hash( "content" ):
            {
              Log::getInstance().error( "PagePseudoElement::findElement", "Unimplemented" );
              return false;
            }
            return true;
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
            Log::getInstance().error( "PagePseudoElement::findElementStaged", "Unimplemented" );
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
        NBBinPseudoElement* tab = nullptr;
        std::unique_ptr< NBBinPseudoElement > tabTemp = nullptr;
        int stack = 0;

        if( this->subject ) {
          tabTemp = std::make_unique< TabPseudoElement >( subject, pageNumber, displayState );
          tab = tabTemp.get();
        } else if ( this->stagedTabElement ) {
          tab = this->stagedTabElement;
        }

        if( tab ) {
          stack = tab->getElementsByClass( L, classID ); // table/none
        }

        // TODO: Content

        return stack;
      }

      /**
       *
       * STACK ARGS: none
       * Returns: userdata, or none
       */
      int PagePseudoElement::getElementById( lua_State* L, const std::string& id ) {
        NBBinPseudoElement* tab = nullptr;
        std::unique_ptr< NBBinPseudoElement > tabTemp = nullptr;

        if( this->subject ) {
          tabTemp = std::make_unique< TabPseudoElement >( subject, pageNumber, displayState );
          tab = tabTemp.get();
        } else if ( this->stagedTabElement ) {
          tab = this->stagedTabElement;
        }

        if( tab ) {
          if( int stack = tab->getElementById( L, id ) ) { // userdata/none
            return stack;
          }
        }

        // TODO: Content

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

        delete widgetPtr;

        return 0;
      }

    }
  }
}
