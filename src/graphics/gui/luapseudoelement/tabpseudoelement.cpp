#include "graphics/gui/luapseudoelement/tabpseudoelement.hpp"
#include "graphics/gui/luaelement.hpp"
#include "graphics/widgetbuilder.hpp"
#include "tools/ctvalidators.hpp"
#include "log.hpp"
#include <tinyxml2.h>

namespace BlueBear {
  namespace Graphics {
    namespace GUI {

      TabPseudoElement::TabPseudoElement( std::shared_ptr< sfg::Notebook > subject, unsigned int pageNumber, Display::MainGameState& displayState )
        : subject( subject ), pageNumber( pageNumber ), displayState( displayState ), stagedWidget( nullptr ) {}

      /**
       *
       * STACK ARGS: userdata
       * (Stack is unmodified after call)
       */
      void TabPseudoElement::setMetatable( lua_State* L ) {
        if( luaL_newmetatable( L, "bluebear_tab_pseudo_element" ) ) { // metatable userdata
          luaL_Reg tableFuncs[] = {
            { "add", TabPseudoElement::lua_add },
            { "remove", TabPseudoElement::lua_removeWidget },
            { "get_name", TabPseudoElement::lua_getName },
            { "find_pseudo", TabPseudoElement::lua_findElement },
            { "find_by_id", TabPseudoElement::lua_findById },
            { "find_by_class", TabPseudoElement::lua_findByClass },
            { "get_property", TabPseudoElement::lua_getProperty },
            { "set_property", TabPseudoElement::lua_setProperty },
            { "get_content", TabPseudoElement::lua_getContent },
            { "set_content", TabPseudoElement::lua_setContent },
            { "__gc", TabPseudoElement::lua_gc },
            { NULL, NULL }
          };

          luaL_setfuncs( L, tableFuncs, 0 );

          lua_pushvalue( L, -1 ); // metatable metatable userdata

          lua_setfield( L, -2, "__index" ); // metatable userdata
        }

        lua_setmetatable( L, -2 ); // userdata
      }

      /**
       * @static
       * Create and push a new unstaged userdata for a <tab> pseudoelement. This can be disconnected/connected to pages before they are staged.
       *
       * STACK ARGS: (none)
       * Returns: userdata, or none
       */
      int TabPseudoElement::create( lua_State* L, Display::MainGameState& displayState, tinyxml2::XMLElement* element ) {
        std::shared_ptr< sfg::Widget > widget = nullptr;
        tinyxml2::XMLElement* child = element->FirstChildElement();

        if( child != NULL ) {
          try {
            WidgetBuilder widgetBuilder( displayState.instance.eventManager, displayState.getImageCache() );
            widget = widgetBuilder.getWidgetFromElementDirect( child );
          } catch( std::exception& e ) {
            Log::getInstance().error( "TabPseudoElement::lua_create", "Failed to add widget XML: " + std::string( e.what() ) );
            return 0;
          }
        }

        TabPseudoElement** userData = ( TabPseudoElement** ) lua_newuserdata( L, sizeof( TabPseudoElement* ) ); // userdata
        *userData = new TabPseudoElement( nullptr, 0, displayState );
        ( *userData )->setMetatable( L );
        ( *userData )->stagedWidget = widget;

        return 1;
      }

      int TabPseudoElement::lua_add( lua_State* L ) {
        TabPseudoElement* self = *( ( TabPseudoElement** ) luaL_checkudata( L, 1, "bluebear_tab_pseudo_element" ) );

        if( self->subject ) {
          Log::getInstance().warn( "TabPseudoElement::lua_add", "Cannot add to <tab> pseudo-element when attached to an existing Notebook." );
        } else if( self->stagedWidget ) {
          Log::getInstance().warn( "TabPseudoElement::lua_add", "Cannot add more than one child widget to <tab> pseudo-element." );
        } else {
          // Empty <tab> pseudo element
          if( lua_isstring( L, -1 ) ) {
            self->setStagedChild( L, lua_tostring( L, -1 ) );
          } else {
            LuaElement* element = *( ( LuaElement** ) luaL_checkudata( L, 2, "bluebear_widget" ) );
            self->setStagedChild( L, element );
          }
        }

        return 0;
      }

      int TabPseudoElement::lua_removeWidget( lua_State* L ) {
        TabPseudoElement* self = *( ( TabPseudoElement** ) luaL_checkudata( L, 1, "bluebear_tab_pseudo_element" ) );

        if( self->subject ) {
          Log::getInstance().warn( "TabPseudoElement::lua_removeWidget", "Cannot remove from <tab> pseudo-element when attached to an existing Notebook." );
        } else if( self->stagedWidget ) {
          LuaElement* argument = *( ( LuaElement** ) luaL_checkudata( L, 2, "bluebear_widget" ) );

          if( argument->widget == self->stagedWidget ) {
            self->stagedWidget = nullptr;
          }
        } else {
          // Empty <tab> pseudo-element, nothing to remove.
          Log::getInstance().debug( "TabPseudoElement::lua_removeWidget", "There is nothing to remove from this <tab> pseudo-element." );
        }

        return 0;
      }

      int TabPseudoElement::lua_getName( lua_State* L ) {
        lua_pushstring( L, "tab" );

        return 1;
      }

      void TabPseudoElement::setStagedChild( lua_State* L, LuaElement* element ) {
        stagedWidget = element->widget;
      }

      void TabPseudoElement::setStagedChild( lua_State* L, const std::string& xmlString ) {
        WidgetBuilder widgetBuilder( displayState.instance.eventManager, displayState.getImageCache() );

        try {
          stagedWidget = widgetBuilder.getWidgetFromXML( xmlString );
        } catch( std::exception& e ) {
          Log::getInstance().error( "LuaElement::add", "Failed to add widget XML: " + std::string( e.what() ) );
        }
      }

      void TabPseudoElement::setSubject( std::shared_ptr< sfg::Notebook > subject ) {
        this->subject = subject;

        stagedWidget = nullptr;
      }

      /**
       *
       * STACK ARGS: none
       * Returns: table, or none
       */
      int TabPseudoElement::getElementsByClass( lua_State* L, const std::string& classID ) {
        std::vector< std::shared_ptr< sfg::Widget > > widgets;

        if( std::shared_ptr< sfg::Widget > child = this->getChildWidget() ) {
          std::vector< std::shared_ptr< sfg::Widget > > widgets = child->GetWidgetsByClass( classID );

          // Add the item itself if it matches the classID
          if( child->GetClass() == classID ) {
            widgets.push_back( child );
          }

          auto size = widgets.size();
          if( size ) {
            lua_createtable( L, size, 0 ); // table

            for( int i = 0; i != size; i++ ) {
              LuaElement::getUserdataFromWidget( L, widgets[ i ] ); // userdata table
              lua_rawseti( L, -2, i + 1 ); // table
            }

            return 1;
          }
        }

        Log::getInstance().warn( "TabPseudoElement::getElementsByClass", "No elements of class " + classID + " found in this pseudo-element." );
        return 0;
      }

      /**
       *
       * STACK ARGS: none
       * Returns: userdata, or none
       */
      int TabPseudoElement::getElementById( lua_State* L, const std::string& id ) {

        if( std::shared_ptr< sfg::Widget > child = this->getChildWidget() ) {

          if( child->GetId() == id ) {
            LuaElement::getUserdataFromWidget( L, child ); // userdata
            return 1;
          }

          if( std::shared_ptr< sfg::Widget > potentialChild = child->GetWidgetById( id ) ) {
            LuaElement::getUserdataFromWidget( L, potentialChild ); // userdata
            return 1;
          }

        }

        Log::getInstance().warn( "TabPseudoElement::getElementById", "No elements of ID " + id + " found in this pseudo-element." );
        return 0;
      }

      std::shared_ptr< sfg::Widget > TabPseudoElement::getChildWidget() {
        if( subject ) {
          return subject->GetNthTabLabel( pageNumber );
        } else if ( stagedWidget ) {
          return stagedWidget;
        } else {
          return nullptr;
        }
      }

      int TabPseudoElement::lua_findById( lua_State* L ) {
        VERIFY_STRING( "TabPseudoElement::lua_findById", "find_by_id" );

        TabPseudoElement* self = *( ( TabPseudoElement** ) luaL_checkudata( L, 1, "bluebear_tab_pseudo_element" ) );

        return self->getElementById( L, lua_tostring( L, -1 ) );
      }

      int TabPseudoElement::lua_findByClass( lua_State* L ) {
        VERIFY_STRING( "TabPseudoElement::lua_findById", "find_by_class" );

        TabPseudoElement* self = *( ( TabPseudoElement** ) luaL_checkudata( L, 1, "bluebear_tab_pseudo_element" ) );

        return self->getElementsByClass( L, lua_tostring( L, -1 ) );
      }

      int TabPseudoElement::lua_findElement( lua_State* L ) {
        Log::getInstance().warn( "TabPseudoElement::lua_findElement", "<tab> pseudo-element has no pseudo-element children." );
        return 0;
      }

      int TabPseudoElement::lua_getProperty( lua_State* L ) {
        Log::getInstance().warn( "TabPseudoElement::lua_getProperty", "<tab> pseudo-element has no gettable properties." );
        return 0;
      }

      int TabPseudoElement::lua_setProperty( lua_State* L ) {
        Log::getInstance().warn( "TabPseudoElement::lua_setProperty", "<tab> pseudo-element has no settable properties." );
        return 0;
      }

      int TabPseudoElement::lua_getContent( lua_State* L ) {
        Log::getInstance().warn( "TabPseudoElement::lua_getContent", "<tab> pseudo-element has no direct content." );
        return 0;
      }

      int TabPseudoElement::lua_setContent( lua_State* L ) {
        Log::getInstance().warn( "TabPseudoElement::lua_setContent", "<tab> pseudo-element has no direct content." );
        return 0;
      }

      int TabPseudoElement::lua_gc( lua_State* L ) {
        TabPseudoElement* widgetPtr = *( ( TabPseudoElement** ) luaL_checkudata( L, 1, "bluebear_tab_pseudo_element" ) );

        delete widgetPtr;

        return 0;
      }


    }
  }
}
