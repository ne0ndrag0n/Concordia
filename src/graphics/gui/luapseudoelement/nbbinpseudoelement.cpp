#include "graphics/gui/luapseudoelement/nbbinpseudoelement.hpp"
#include "graphics/gui/luaelement.hpp"
#include "graphics/widgetbuilder.hpp"
#include "tools/ctvalidators.hpp"
#include "eventmanager.hpp"
#include "log.hpp"
#include <tinyxml2.h>

namespace BlueBear {
  namespace Graphics {
    namespace GUI {

      NBBinPseudoElement::NBBinPseudoElement( std::shared_ptr< sfg::Notebook > subject, unsigned int pageNumber, Display::MainGameState& displayState )
        : subject( subject ), pageNumber( pageNumber ), displayState( displayState ), stagedWidget( nullptr ) {
        listen();
      }

      NBBinPseudoElement::~NBBinPseudoElement() {
        deafen();
      }

      /**
       *
       * STACK ARGS: userdata
       * (Stack is unmodified after call)
       */
      void NBBinPseudoElement::setMetatable( lua_State* L ) {
        if( luaL_newmetatable( L, "bluebear_nbb_pseudo_element" ) ) { // metatable userdata
          luaL_Reg tableFuncs[] = {
            { "add", NBBinPseudoElement::lua_add },
            { "remove", NBBinPseudoElement::lua_removeWidget },
            { "get_name", NBBinPseudoElement::lua_getName },
            { "find_pseudo", NBBinPseudoElement::lua_findElement },
            { "find_by_id", NBBinPseudoElement::lua_findById },
            { "find_by_class", NBBinPseudoElement::lua_findByClass },
            { "get_property", NBBinPseudoElement::lua_getProperty },
            { "set_property", NBBinPseudoElement::lua_setProperty },
            { "get_content", NBBinPseudoElement::lua_getContent },
            { "set_content", NBBinPseudoElement::lua_setContent },
            { "__gc", NBBinPseudoElement::lua_gc },
            { NULL, NULL }
          };

          luaL_setfuncs( L, tableFuncs, 0 );

          lua_pushvalue( L, -1 ); // metatable metatable userdata

          lua_setfield( L, -2, "__index" ); // metatable userdata
        }

        lua_setmetatable( L, -2 ); // userdata
      }

      /**
       * Linker pitches a fit if this isn't here
       */
      std::string NBBinPseudoElement::getName() {
        return "<invalid>";
      }

      void NBBinPseudoElement::listen() {
        eventManager.ITEM_ADDED.listen( this, std::bind( &NBBinPseudoElement::onItemAdded, this, std::placeholders::_1, std::placeholders::_2 ) );
        eventManager.ITEM_REMOVED.listen( this, std::bind( &NBBinPseudoElement::onItemRemoved, this, std::placeholders::_1, std::placeholders::_2 ) );
      }

      void NBBinPseudoElement::deafen() {
        eventManager.ITEM_ADDED.stopListening( this );
        eventManager.ITEM_REMOVED.stopListening( this );
      }

      void NBBinPseudoElement::onItemAdded( void* notebook, int changed ) {
        // TODO
      }

      void NBBinPseudoElement::onItemRemoved( void* notebook, int changed ) {
        if( subject.get() == notebook && pageNumber > changed ) {
          Log::getInstance().debug( "NBBinPseudoElement of type " + getName(), "Picked up the remove event and turning item " + std::to_string( pageNumber ) + " to " + std::to_string( pageNumber - 1 ) );
          --pageNumber;
        }
      }

      std::shared_ptr< sfg::Widget > NBBinPseudoElement::createStagedWidget( Display::MainGameState& displayState, tinyxml2::XMLElement* element ) {
        tinyxml2::XMLElement* child = element->FirstChildElement();

        if( child != NULL ) {
          try {
            WidgetBuilder widgetBuilder( displayState.getImageCache() );
            return widgetBuilder.getWidgetFromElementDirect( child );
          } catch( std::exception& e ) {
            Log::getInstance().error( "NBBinPseudoElement::create", "Failed to add widget XML: " + std::string( e.what() ) );
          }
        }

        return nullptr;
      }

      int NBBinPseudoElement::lua_add( lua_State* L ) {
        NBBinPseudoElement* self = *( ( NBBinPseudoElement** ) luaL_checkudata( L, 1, "bluebear_nbb_pseudo_element" ) );

        if( self->subject ) {
          Log::getInstance().warn( "NBBinPseudoElement::lua_add", "Cannot add to <" + self->getName() + "> pseudo-element when attached to an existing Notebook." );
        } else if( self->stagedWidget ) {
          Log::getInstance().warn( "NBBinPseudoElement::lua_add", "Cannot add more than one child widget to <" + self->getName() + "> pseudo-element." );
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

      int NBBinPseudoElement::lua_removeWidget( lua_State* L ) {
        NBBinPseudoElement* self = *( ( NBBinPseudoElement** ) luaL_checkudata( L, 1, "bluebear_nbb_pseudo_element" ) );

        if( self->subject ) {
          Log::getInstance().warn( "NBBinPseudoElement::lua_removeWidget", "Cannot remove from <" + self->getName() + "> pseudo-element when attached to an existing Notebook." );
        } else if( self->stagedWidget ) {
          LuaElement* argument = *( ( LuaElement** ) luaL_checkudata( L, 2, "bluebear_widget" ) );

          if( argument->widget == self->stagedWidget ) {
            self->stagedWidget = nullptr;
          }
        } else {
          // Empty <tab> pseudo-element, nothing to remove.
          Log::getInstance().debug( "NBBinPseudoElement::lua_removeWidget", "There is nothing to remove from this <" + self->getName() + "> pseudo-element." );
        }

        return 0;
      }

      int NBBinPseudoElement::lua_getName( lua_State* L ) {
        NBBinPseudoElement* self = *( ( NBBinPseudoElement** ) luaL_checkudata( L, 1, "bluebear_nbb_pseudo_element" ) );

        lua_pushstring( L, self->getName().c_str() );

        return 1;
      }

      void NBBinPseudoElement::setStagedChild( lua_State* L, LuaElement* element ) {
        stagedWidget = element->widget;
      }

      void NBBinPseudoElement::setStagedChild( lua_State* L, const std::string& xmlString ) {
        WidgetBuilder widgetBuilder( displayState.getImageCache() );

        try {
          stagedWidget = widgetBuilder.getWidgetFromXML( xmlString );
        } catch( std::exception& e ) {
          Log::getInstance().error( "LuaElement::add", "Failed to add widget XML: " + std::string( e.what() ) );
        }
      }

      void NBBinPseudoElement::setSubject( std::shared_ptr< sfg::Notebook > subject, unsigned int pageNumber ) {
        this->pageNumber = pageNumber;
        this->subject = subject;

        stagedWidget = nullptr;
      }

      /**
       *
       * STACK ARGS: none
       * Returns: table, or none
       */
      int NBBinPseudoElement::getElementsByClass( lua_State* L, const std::string& classID ) {
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

        Log::getInstance().warn( "NBBinPseudoElement::getElementsByClass", "No elements of class " + classID + " found in this pseudo-element." );
        return 0;
      }

      /**
       *
       * STACK ARGS: none
       * Returns: userdata, or none
       */
      int NBBinPseudoElement::getElementById( lua_State* L, const std::string& id ) {

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

        Log::getInstance().warn( "NBBinPseudoElement::getElementById", "No elements of ID " + id + " found in this pseudo-element." );
        return 0;
      }

      std::shared_ptr< sfg::Widget > NBBinPseudoElement::getChildWidget() {
        if( subject ) {
          return this->getSubjectChildWidget();
        } else if ( stagedWidget ) {
          return stagedWidget;
        } else {
          return nullptr;
        }
      }

      int NBBinPseudoElement::lua_findById( lua_State* L ) {
        VERIFY_STRING( "NBBinPseudoElement::lua_findById", "find_by_id" );

        NBBinPseudoElement* self = *( ( NBBinPseudoElement** ) luaL_checkudata( L, 1, "bluebear_nbb_pseudo_element" ) );

        return self->getElementById( L, lua_tostring( L, -1 ) );
      }

      int NBBinPseudoElement::lua_findByClass( lua_State* L ) {
        VERIFY_STRING( "NBBinPseudoElement::lua_findById", "find_by_class" );

        NBBinPseudoElement* self = *( ( NBBinPseudoElement** ) luaL_checkudata( L, 1, "bluebear_nbb_pseudo_element" ) );

        return self->getElementsByClass( L, lua_tostring( L, -1 ) );
      }

      int NBBinPseudoElement::lua_findElement( lua_State* L ) {
        Log::getInstance().warn( "NBBinPseudoElement::lua_findElement", "<tab> and <page> pseudo-element has no pseudo-element children." );
        return 0;
      }

      int NBBinPseudoElement::lua_getProperty( lua_State* L ) {
        Log::getInstance().warn( "NBBinPseudoElement::lua_getProperty", "<tab> and <page> pseudo-element has no gettable properties." );
        return 0;
      }

      int NBBinPseudoElement::lua_setProperty( lua_State* L ) {
        Log::getInstance().warn( "NBBinPseudoElement::lua_setProperty", "<tab> and <page> pseudo-element has no settable properties." );
        return 0;
      }

      int NBBinPseudoElement::lua_getContent( lua_State* L ) {
        Log::getInstance().warn( "NBBinPseudoElement::lua_getContent", "<tab> and <page> pseudo-element has no direct content." );
        return 0;
      }

      int NBBinPseudoElement::lua_setContent( lua_State* L ) {
        Log::getInstance().warn( "NBBinPseudoElement::lua_setContent", "<tab> and <page> pseudo-element has no direct content." );
        return 0;
      }

      int NBBinPseudoElement::lua_gc( lua_State* L ) {
        NBBinPseudoElement* widgetPtr = *( ( NBBinPseudoElement** ) luaL_checkudata( L, 1, "bluebear_nbb_pseudo_element" ) );

        delete widgetPtr;

        return 0;
      }


    }
  }
}
