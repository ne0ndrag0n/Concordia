#include "graphics/gui/luaelement.hpp"
#include "graphics/gui/luapseudoelement/pagepseudoelement.hpp"
#include "graphics/gui/luapseudoelement/itempseudoelement.hpp"
#include "graphics/gui/luapseudoelement/rowpseudoelement.hpp"
#include "graphics/widgetbuilder.hpp"
#include "graphics/imagebuilder/pathimagesource.hpp"
#include "graphics/display.hpp"
#include "tools/ctvalidators.hpp"
#include "tools/utility.hpp"
#include "configmanager.hpp"
#include "eventmanager.hpp"
#include "log.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFGUI/SFGUI.hpp>
#include <string>

namespace BlueBear {
  namespace Graphics {
    namespace GUI {

      std::map< std::shared_ptr< sfg::Widget >, std::map< sfg::Signal::SignalID, LuaElement::SignalBinding > > LuaElement::masterSignalMap;
      std::map< std::shared_ptr< sfg::Widget >, std::map< std::string, std::string > > LuaElement::masterAttrMap;

      void LuaElement::add( lua_State* L, const std::string& xmlString, Display::MainGameState& state ) {
        if( isContainer() ) {
          tinyxml2::XMLDocument document;
          if( tinyxml2::XMLElement* element = Tools::Utility::getRootNode( document, xmlString ) ) {
            // Check for pseudo-elements
            switch( Tools::Utility::hash( element->Name() ) ) {
              case Tools::Utility::hash( "page" ): {
                if( PagePseudoElement::create( L, state, element ) ) { // userdata
                  add( L, *( ( PagePseudoElement** ) lua_topointer( L, -1 ) ) );
                  lua_pop( L, 1 ); // EMPTY
                }
                break;
              }
              case Tools::Utility::hash( "item" ): {
                if( ItemPseudoElement::create( L, state, element ) ) { // userdata
                  add( L, *( ( ItemPseudoElement** ) lua_topointer( L, -1 ) ) );
                  lua_pop( L, 1 ); // EMPTY
                }
                break;
              }
              default: {
                WidgetBuilder widgetBuilder( state.instance.eventManager, state.getImageCache() );
                std::shared_ptr< sfg::Widget > child = nullptr;

                try {
                  child = widgetBuilder.getWidgetFromElementDirect( element );
                } catch( std::exception& e ) {
                  Log::getInstance().error( "LuaElement::add", "Failed to add widget XML: " + std::string( e.what() ) );
                  return;
                }

                // Add to widget typecast as container
                addToCheckedContainer( child );
              }
            }
          }
        } else {
          Log::getInstance().warn( "LuaElement::add", "This LuaElement is not a Container and cannot be added to." );
        }
      }

      void LuaElement::add( LuaElement* element ) {
        if( isContainer() ) {
          addToCheckedContainer( element->widget );
        } else {
          Log::getInstance().warn( "LuaElement::add", "This LuaElement is not a Container and cannot be added to." );
        }
      }

      void LuaElement::add( lua_State* L, PagePseudoElement* page ) {
        switch( Tools::Utility::hash( widget->GetName().c_str() ) ) {
          case Tools::Utility::hash( "Notebook" ): {
            page->setSubject( L, std::static_pointer_cast< sfg::Notebook >( widget ) );
            break;
          }
          default:
            Log::getInstance().debug( "LuaElement::add", "Cannot add a <" + page->getName() + "> pseudo-element to a " + widget->GetName() + " widget." );
        }
      }

      void LuaElement::add( lua_State* L, ItemPseudoElement* item ) {
        switch( Tools::Utility::hash( widget->GetName().c_str() ) ) {
          case Tools::Utility::hash( "ComboBox" ): {
            item->setSubject( std::static_pointer_cast< sfg::ComboBox >( widget ) );
            break;
          }
          default:
            Log::getInstance().debug( "LuaElement::add", "Cannot add a <" + item->getName() + "> pseudo-element to a " + widget->GetName() + " widget." );
        }
      }

      void LuaElement::addToCheckedContainer( std::shared_ptr< sfg::Widget > target ) {
        if( std::string( widget->GetName() ) == "Box" ) {
          std::shared_ptr< sfg::Box > widgetAsBox = std::static_pointer_cast< sfg::Box >( widget );
          widgetAsBox->PackEnd( target, true, true );
        } else {
          std::shared_ptr< sfg::Container > widgetAsContainer = std::static_pointer_cast< sfg::Container >( widget );
          widgetAsContainer->Add( target );
        }
      }

      void LuaElement::removeWidget( std::shared_ptr< sfg::Widget > target ) {
        if( isContainer() ) {
          std::shared_ptr< sfg::Container > widget = std::static_pointer_cast< sfg::Container >( widget );

          target->Show( false );
          widget->Remove( target );
        } else {
          Log::getInstance().warn( "LuaElement::removeWidget", "This LuaElement is not a Container and cannot remove any child elements." );
        }
      }

      void LuaElement::removePseudoElement( PagePseudoElement* page ) {
        page->removeFromNotebook( widget );
      }

      /**
       * TODO: This is now identical to Tools::Utility::widgetIsContainer, consider replacing this method with that one if any changes required.
       */
      bool LuaElement::isContainer() {
        switch( Tools::Utility::hash( widget->GetName().c_str() ) ) {
          case Tools::Utility::hash( "Box" ):
          case Tools::Utility::hash( "Fixed" ):
          case Tools::Utility::hash( "Notebook" ):
          case Tools::Utility::hash( "ScrolledWindow" ):
          case Tools::Utility::hash( "Table" ):
          case Tools::Utility::hash( "Alignment" ):
          //case Tools::Utility::hash( "Button" ):
          //case Tools::Utility::hash( "ToggleButton" ):
          //case Tools::Utility::hash( "CheckButton" ):
          //case Tools::Utility::hash( "RadioButton" ):
          //case Tools::Utility::hash( "ComboBox" ):
          case Tools::Utility::hash( "Frame" ):
          case Tools::Utility::hash( "Viewport" ):
          case Tools::Utility::hash( "Window" ):
            return true;
          default:
            return false;
        }
      }

      /**
       * @static
       */
      int LuaElement::lua_onEvent( lua_State* L ) {
        LuaElement** userData = ( LuaElement** ) luaL_checkudata( L, 1, "bluebear_widget" );
        Display::MainGameState* self = ( Display::MainGameState* )lua_touserdata( L, lua_upvalueindex( 1 ) );
        LuaElement& element = **userData;

        // FIXME: argument -1 needs verification for function type

        // function "event" self

        if( lua_isstring( L, -2 ) ) {
          const char* eventType = lua_tostring( L, -2 );

          switch( Tools::Utility::hash( eventType ) ) {
            case Tools::Utility::hash( "click" ):
              {
                // Create the bucket for this widget if it doesn't exist, otherwise, return a new bucket
                auto& signalMap = masterSignalMap[ element.widget ];

                unregisterClickHandler( L, signalMap, element.widget );

                // Track the master reference
                // Unref this if the pointer is ever removed!
                LuaReference masterReference = luaL_ref( L, LUA_REGISTRYINDEX ); // "event" self
                signalMap[ sfg::Widget::OnLeftClick ] = LuaElement::SignalBinding{
                  masterReference,
                  element.widget->GetSignal( sfg::Widget::OnLeftClick ).Connect( std::bind( LuaElement::clickHandler, L, self->instance.eventManager, element.widget, masterReference, "left" ) )
                };
                signalMap[ sfg::Widget::OnRightClick ] = LuaElement::SignalBinding{
                  masterReference,
                  element.widget->GetSignal( sfg::Widget::OnRightClick ).Connect( std::bind( LuaElement::clickHandler, L, self->instance.eventManager, element.widget, masterReference, "right" ) )
                };

                lua_pushboolean( L, true ); // true "event" self
                return 1; // true
              }
            case Tools::Utility::hash( "mouse_enter" ):
              {
                registerGenericHandler( L, self->instance.eventManager, element.widget, sfg::Widget::OnMouseEnter ); // true "event" self
                return 1; // true
              }
            case Tools::Utility::hash( "mouse_leave" ):
              {
                registerGenericHandler( L, self->instance.eventManager, element.widget, sfg::Widget::OnMouseLeave ); // true "event" self
                return 1; // true
              }
            case Tools::Utility::hash( "focus" ):
              {
                registerGenericHandler( L, self->instance.eventManager, element.widget, sfg::Widget::OnGainFocus ); // true "event" self
                return 1; // true
              }
            case Tools::Utility::hash( "blur" ):
              {
                registerGenericHandler( L, self->instance.eventManager, element.widget, sfg::Widget::OnLostFocus ); // true "event" self
                return 1; // true
              }
            default:
              Log::getInstance().warn( "LuaElement::lua_onEvent", "Invalid event type specified: " + std::string( eventType ) );
          }
        } else {
          Log::getInstance().warn( "LuaElement::lua_onEvent", "Invalid event type passed to on()" );
        }

        return 0;
      }

      /**
       * @static
       */
      int LuaElement::lua_offEvent( lua_State* L ) {
        LuaElement** userData = ( LuaElement** ) luaL_checkudata( L, 1, "bluebear_widget" );
        LuaElement& element = **userData;

        // "event type" self
        if( lua_isstring( L, -1 ) ) {

          const char* eventType = lua_tostring( L, -1 );
          auto signalMap = masterSignalMap.find( element.widget );
          if( signalMap != masterSignalMap.end() ) {

            switch( Tools::Utility::hash( eventType ) ) {
              case Tools::Utility::hash( "click" ):
                unregisterClickHandler( L, signalMap->second, element.widget );
                break;
              case Tools::Utility::hash( "mouse_enter" ):
                unregisterHandler( L, signalMap->second, element.widget, sfg::Widget::OnMouseEnter );
                break;
              case Tools::Utility::hash( "mouse_leave" ):
                unregisterHandler( L, signalMap->second, element.widget, sfg::Widget::OnMouseLeave );
                break;
              case Tools::Utility::hash( "focus" ):
                unregisterHandler( L, signalMap->second, element.widget, sfg::Widget::OnGainFocus );
                break;
              case Tools::Utility::hash( "blur" ):
                unregisterHandler( L, signalMap->second, element.widget, sfg::Widget::OnLostFocus );
                break;
              default:
                Log::getInstance().warn( "LuaElement::lua_offEvent", "Invalid event type specified: " + std::string( eventType ) );
            }

          }

        } else {
          Log::getInstance().warn( "LuaElement::lua_offEvent", "Invalid event type passed to off()" );
        }

        return 0;
      }

      /**
       * @static
       */
      int LuaElement::lua_getWidgetByID( lua_State* L ) {
        std::string selector;

        if( lua_isstring( L, -1 ) ) {
          selector = std::string( lua_tostring( L, -1 ) );
        } else {
          Log::getInstance().warn( "LuaElement::lua_getWidgetByID", "Argument provided to get_widget_by_id was not a string." );
          return 0;
        }

        LuaElement** userData = ( LuaElement** ) luaL_checkudata( L, 1, "bluebear_widget" );
        std::shared_ptr< sfg::Widget > parentWidget = ( *userData )->widget;

        // Holy moly is this going to get confusing quick
        // I also have the least amount of confidence in this code you could possibly imagine
        std::shared_ptr< sfg::Widget > widget = parentWidget->GetWidgetById( selector );
        if( widget ) {
          getUserdataFromWidget( L, widget ); // userdata
          return 1;
        } else {
          std::string error = std::string( "Could not find widget with ID " ) + selector;
          return luaL_error( L, error.c_str() );
        }
      }

      /**
       * @static
       */
      int LuaElement::lua_getWidgetsByClass( lua_State* L ) {
        std::string selector;

        // Check for presence of string selector
        if( lua_isstring( L, -1 ) ) {
          selector = lua_tostring( L, -1 );
        } else {
          Log::getInstance().warn( "LuaElement::lua_getWidgetsByClass", "Argument provided to get_widgets_by_class was not a string." );
          return 0;
        }

        LuaElement** userData = ( LuaElement** ) luaL_checkudata( L, 1, "bluebear_widget" );
        std::shared_ptr< sfg::Widget > parentWidget = ( *userData )->widget;

        // Perform the actual SFGUI call
        sfg::Widget::WidgetsList widgets = parentWidget->GetWidgetsByClass( selector );

        auto size = widgets.size();
        if( size ) {
          // At least one widget is present - these items must be chained in a Lua table
          // Create a Lua table, then push a new LuaInstance wrapper for every widget found as part of this class
          lua_createtable( L, size, 0 ); // table

          for( int i = 0; i != size; i++ ) {
            getUserdataFromWidget( L, widgets[ i ] ); // userdata table
            lua_rawseti( L, -2, i + 1 ); // table
          }

          return 1;
        } else {
          // No widgets are present
          std::string error = std::string( "Could not find any widgets with class " + selector );
          return luaL_error( L, error.c_str() );
        }
      }

      /**
       *
       * STACK ARGS: none
       * RETURNS: userdata, array, or none
       */
      bool LuaElement::getPseudoElements( lua_State* L, const std::string& pseudo, Display::MainGameState& state, int index ) {
        // more shitty patching on top of the lack of polymorphism in LuaElement

        switch( Tools::Utility::hash( pseudo.c_str() ) ) {
          case Tools::Utility::hash( "page" ):
            if( widget->GetName() == "Notebook" ) {
              std::shared_ptr< sfg::Notebook > notebook = std::static_pointer_cast< sfg::Notebook >( widget );
              int pageCount = notebook->GetPageCount();

              if( index < 0 ) {
                // Array of PagePseudoElement

                lua_createtable( L, pageCount, 0 ); // table

                for( int i = 0; i != notebook->GetPageCount(); i++ ) {

                  PagePseudoElement** ppe = ( PagePseudoElement** )lua_newuserdata( L, sizeof( PagePseudoElement* ) ); // userdata table
                  *ppe = new PagePseudoElement( notebook, i, state );
                  ( *ppe )->setMetatable( L );

                  lua_rawseti( L, -2, i + 1 ); // table
                }

              } else if( index < pageCount ) {

                PagePseudoElement** ppe = ( PagePseudoElement** )lua_newuserdata( L, sizeof( PagePseudoElement* ) ); // userdata
                *ppe = new PagePseudoElement( notebook, index, state );
                ( *ppe )->setMetatable( L );
              } else {

                Log::getInstance().warn( "LuaElement::getPseudoElements", "Page does not exist in this Notebook." );
                return false;
              }

              return true;
            }
          case Tools::Utility::hash( "item" ): {
            if( widget->GetName() == "ComboBox" ) {
              std::shared_ptr< sfg::ComboBox > comboBox = std::static_pointer_cast< sfg::ComboBox >( widget );
              int itemCount = comboBox->GetItemCount();

              if( index < 0 ) {
                // Array of ItemPseudoElement

                lua_createtable( L, itemCount, 0 ); // table

                for( int i = 0; i != itemCount; i++ ) {
                  ItemPseudoElement** item = ( ItemPseudoElement** ) lua_newuserdata( L, sizeof( ItemPseudoElement* ) ); // userdata table
                  *item = new ItemPseudoElement( comboBox, i, state );
                  ( *item )->setMetatable( L );

                  lua_rawseti( L, -2, i + 1 ); // table
                }
              } else if( index < itemCount ) {
                ItemPseudoElement** item = ( ItemPseudoElement** ) lua_newuserdata( L, sizeof( ItemPseudoElement* ) ); // userdata
                *item = new ItemPseudoElement( comboBox, index, state );
                ( *item )->setMetatable( L );
              } else {
                Log::getInstance().warn( "LuaElement::getPseudoElements", "Item does not exist in this ComboBox." );
                return false;
              }

              return true;
            }
          }
          case Tools::Utility::hash( "row" ) : {
            if( widget->GetName() == "Table" ) {
              std::shared_ptr< sfg::Table > table = std::static_pointer_cast< sfg::Table >( widget );
              int rowCount = RowPseudoElement::getRowCount( table );

              if( index < 0 ) {
                lua_createtable( L, rowCount, 0 ); // table

                for( int i = 0; i != rowCount; i++ ) {
                  RowPseudoElement** row = ( RowPseudoElement** ) lua_newuserdata( L, sizeof( RowPseudoElement* ) ); // userdata table
                  *row = new RowPseudoElement( table, i, state );
                  ( *row )->setMetatable( L );

                  lua_rawseti( L, -2, i + 1 ); // table
                }
              } else if( index < rowCount ) {
                RowPseudoElement** row = ( RowPseudoElement** ) lua_newuserdata( L, sizeof( RowPseudoElement* ) ); // userdata
                *row = new RowPseudoElement( table, index, state );
                ( *row )->setMetatable( L );
              } else {
                Log::getInstance().warn( "LuaElement::getPseudoElements", "Row does not exist in this Table." );
                return false;
              }

              return true;
            }
          }
          default:
            Log::getInstance().warn( "LuaElement::getPseudoElements", widget->GetName() + " has no pseudo-element of type " + pseudo );
            return false;
        }
      }

      int LuaElement::lua_getPseudoElements( lua_State* L ) {
        // index "selector" self
        // or
        // "selector" self
        LuaElement* self = *( ( LuaElement** ) luaL_checkudata( L, 1, "bluebear_widget" ) );

        if( !lua_isstring( L, 2 ) ) {
          Log::getInstance().warn( "LuaElement::lua_getPseudoElements", "Argument 2 provided to find_pseudo must be a string." );
          return 0;
        }

        Display::MainGameState* state = ( Display::MainGameState* )lua_touserdata( L, lua_upvalueindex( 1 ) );

        bool success = lua_isnumber( L, 3 ) ?
          self->getPseudoElements( L, lua_tostring( L, 2 ), *state, lua_tonumber( L, 3 ) ) :
          self->getPseudoElements( L, lua_tostring( L, 2 ), *state ); // element or nothing

        return success ? 1 : 0;
      }

      int LuaElement::lua_getName( lua_State* L ) {
        LuaElement* self = *( ( LuaElement** ) luaL_checkudata( L, 1, "bluebear_widget" ) );

        lua_pushstring( L, self->widget->GetName().c_str() );

        return 1;
      }

      /**
       * @static
       */
      int LuaElement::lua_getText( lua_State* L ) {
        LuaElement* widgetPtr = *( ( LuaElement** ) luaL_checkudata( L, 1, "bluebear_widget" ) );

        std::string widgetType = widgetPtr->widget->GetName();
        switch( Tools::Utility::hash( widgetType.c_str() ) ) {
          case Tools::Utility::hash( "Entry" ):
            {
              std::shared_ptr< sfg::Entry > entry = std::static_pointer_cast< sfg::Entry >( widgetPtr->widget );
              // See what happens when you try to reinvent the string??
              lua_pushstring( L, std::string( entry->GetText() ).c_str() ); // "entry"
            }
            break;
          case Tools::Utility::hash( "SpinButton" ):
            {
              std::shared_ptr< sfg::SpinButton > spinButton = std::static_pointer_cast< sfg::SpinButton >( widgetPtr->widget );
              lua_pushnumber( L, spinButton->GetValue() ); // 42.0
            }
            break;
          case Tools::Utility::hash( "Label" ):
            {
              std::shared_ptr< sfg::Label > label = std::static_pointer_cast< sfg::Label >( widgetPtr->widget );
              lua_pushstring( L, std::string( label->GetText() ).c_str() ); // "entry"
            }
            break;
          case Tools::Utility::hash( "Button" ):
          case Tools::Utility::hash( "ToggleButton" ):
          case Tools::Utility::hash( "CheckButton" ):
          case Tools::Utility::hash( "RadioButton" ):
            {
              std::shared_ptr< sfg::Button > button = std::static_pointer_cast< sfg::Button >( widgetPtr->widget );
              lua_pushstring( L, std::string( button->GetLabel() ).c_str() ); // "entry"
            }
            break;
          case Tools::Utility::hash( "Scrollbar" ):
          case Tools::Utility::hash( "Scale" ):
            {
              std::shared_ptr< sfg::Range > range = std::static_pointer_cast< sfg::Range >( widgetPtr->widget );
              lua_pushnumber( L, range->GetValue() ); // 42.0
            }
            break;
          case Tools::Utility::hash( "ComboBox" ):
            {
              std::shared_ptr< sfg::ComboBox > comboBox = std::static_pointer_cast< sfg::ComboBox >( widgetPtr->widget );
              lua_pushstring( L, std::string( comboBox->GetSelectedText() ).c_str() ); // "entry"
            }
            break;
          default:
            {
              Log::getInstance().warn( "LuaElement::lua_getText", "Object of type " + widgetType + " has no convertible \"text\" field." );
              lua_pushstring( L, "" ); // ""
            }
        }

        // Better have a string ready to return to the user here
        return 1;
      }

      /**
       * @static
       */
      int LuaElement::lua_setText( lua_State* L ) {
        // "text" self

        LuaElement* widgetPtr = *( ( LuaElement** ) luaL_checkudata( L, 1, "bluebear_widget" ) );
        std::string widgetType = widgetPtr->widget->GetName();

        switch( Tools::Utility::hash( widgetType.c_str() ) ) {
          case Tools::Utility::hash( "Entry" ):
            {
              std::shared_ptr< sfg::Entry > entry = std::static_pointer_cast< sfg::Entry >( widgetPtr->widget );
              entry->SetText( lua_tostring( L, -1 ) );
            }
            break;
          case Tools::Utility::hash( "SpinButton" ):
            {
              std::shared_ptr< sfg::SpinButton > spinButton = std::static_pointer_cast< sfg::SpinButton >( widgetPtr->widget );
              spinButton->SetValue( lua_tonumber( L, -1 ) );
            }
            break;
          case Tools::Utility::hash( "Label" ):
            {
              std::shared_ptr< sfg::Label > label = std::static_pointer_cast< sfg::Label >( widgetPtr->widget );
              label->SetText( lua_tostring( L, -1 ) );
            }
            break;
          case Tools::Utility::hash( "Button" ):
          case Tools::Utility::hash( "ToggleButton" ):
          case Tools::Utility::hash( "CheckButton" ):
          case Tools::Utility::hash( "RadioButton" ):
            {
              std::shared_ptr< sfg::Button > button = std::static_pointer_cast< sfg::Button >( widgetPtr->widget );
              button->SetLabel( lua_tostring( L, -1 ) );
            }
            break;
          case Tools::Utility::hash( "Scrollbar" ):
          case Tools::Utility::hash( "Scale" ):
            {
              std::shared_ptr< sfg::Range > range = std::static_pointer_cast< sfg::Range >( widgetPtr->widget );
              range->SetValue( lua_tonumber( L, -1 ) );
            }
            break;
          default:
            {
              Log::getInstance().warn( "LuaElement::lua_setText", "Object of type " + widgetType + " has no settable \"text\" field." );
            }
        }

        return 0;
      }

      /**
       * @static
       */
      int LuaElement::lua_setImage( lua_State* L ) {
        // "image" self
        if( !lua_isstring( L, -1 ) ) {
          Log::getInstance().warn( "LuaElement::lua_setImage", "Argument 1 of set_image must be a string." );
          return 0;
        }

        LuaElement** userData = ( LuaElement** ) luaL_checkudata( L, 1, "bluebear_widget" );
        Display::MainGameState* state = ( Display::MainGameState* )lua_touserdata( L, lua_upvalueindex( 1 ) );
        LuaElement& element = **userData;

        std::string widgetType = element.widget->GetName();
        std::string path = lua_tostring( L, -1 );

        switch( Tools::Utility::hash( widgetType.c_str() ) ) {
          case Tools::Utility::hash( "Image" ):
            {
              std::shared_ptr< sfg::Image > image = std::static_pointer_cast< sfg::Image >( element.widget );

              try {
                PathImageSource piss = PathImageSource( path );
                image->SetImage( *state->getImageCache().getImage( piss ) );
              } catch( std::exception& e ) {
                std::string error = "Could not load image for image widget: " + std::string( e.what() );
                Log::getInstance().warn( "LuaElement::lua_setImage", error );
                return luaL_error( L, error.c_str() );
              }

            }
            return 0;
          default:
            Log::getInstance().warn( "LuaElement::lua_setImage", "Object of type " + widgetType + " has no convertible image type." );
        }

        return 0;
      }

      /**
       * @static
       */
      int LuaElement::lua_getProperty( lua_State* L ) {
        LuaElement* widgetPtr = *( ( LuaElement** ) luaL_checkudata( L, 1, "bluebear_widget" ) );

        // "property" self

        if( !lua_isstring( L, -1 ) ) {
          Log::getInstance().warn( "LuaElement::lua_getProperty", "Argument 1 of get_property must be a string." );
          return 0;
        }

        std::string widgetType = widgetPtr->widget->GetName();

        const char* property = lua_tostring( L, -1 );
        switch( Tools::Utility::hash( property ) ) {
          case Tools::Utility::hash( "visible" ):
           {
             lua_pushboolean( L, widgetPtr->widget->IsLocallyVisible() ? 1 : 0 ); // true/false
             return 1;
           }
          case Tools::Utility::hash( "id" ):
            {
              lua_pushstring( L, widgetPtr->widget->GetId().c_str() ); // "id"
              return 1;
            }
          case Tools::Utility::hash( "class" ):
            {
              lua_pushstring( L, widgetPtr->widget->GetClass().c_str() ); // "class"
              return 1;
            }
          case Tools::Utility::hash( "min-width" ):
            {
              sf::Vector2f requisition = widgetPtr->widget->GetRequisition();
              lua_pushnumber( L, requisition.x ); // 42.0
              return 1;
            }
          case Tools::Utility::hash( "min-height" ):
            {
              sf::Vector2f requisition = widgetPtr->widget->GetRequisition();
              lua_pushnumber( L, requisition.y ); // 42.0
              return 1;
            }
          case Tools::Utility::hash( "width" ):
            {
              sf::FloatRect allocation = widgetPtr->widget->GetAllocation();
              lua_pushnumber( L, allocation.width ); // 42.0
              return 1;
            }
          case Tools::Utility::hash( "height" ):
            {
              sf::FloatRect allocation = widgetPtr->widget->GetAllocation();
              lua_pushnumber( L, allocation.height ); // 42.0
              return 1;
            }
          case Tools::Utility::hash( "left" ):
            {
              sf::FloatRect allocation = widgetPtr->widget->GetAllocation();
              lua_pushnumber( L, allocation.left ); // 42.0
              return 1;
            }
          case Tools::Utility::hash( "top" ):
            {
              sf::FloatRect allocation = widgetPtr->widget->GetAllocation();
              lua_pushnumber( L, allocation.top ); // 42.0
              return 1;
            }
          case Tools::Utility::hash( "title" ):
            {
              // Verify this is applicable for the given widget
              if( widgetType == "Window" ) {
                std::shared_ptr< sfg::Window > window = std::static_pointer_cast< sfg::Window >( widgetPtr->widget );
                lua_pushstring( L, std::string( window->GetTitle() ).c_str() ); // "title"
                return 1;
              } else if ( widgetType == "Frame" ) {
                std::shared_ptr< sfg::Frame > frame = std::static_pointer_cast< sfg::Frame >( widgetPtr->widget );
                lua_pushstring( L, std::string( frame->GetLabel() ).c_str() ); // "title"
                return 1;
              } else {
                Log::getInstance().warn( "LuaElement::lua_getProperty", "Property \"" + std::string( property ) + "\" does not exist for this widget type." );
                return 0;
              }
            }
          case Tools::Utility::hash( "titlebar" ):
            {
              if( widgetType != "Window" ) {
                Log::getInstance().warn( "LuaElement::lua_getProperty", "Property \"" + std::string( property ) + "\" does not exist for this widget type." );
                return 0;
              }

              std::shared_ptr< sfg::Window > window = std::static_pointer_cast< sfg::Window >( widgetPtr->widget );
              lua_pushboolean( L, window->HasStyle( sfg::Window::Style::TITLEBAR ) ? 1 : 0 ); // true/false
              return 1;
            }
          case Tools::Utility::hash( "background" ):
            {
              if( widgetType != "Window" ) {
                Log::getInstance().warn( "LuaElement::lua_getProperty", "Property \"" + std::string( property ) + "\" does not exist for this widget type." );
                return 0;
              }

              std::shared_ptr< sfg::Window > window = std::static_pointer_cast< sfg::Window >( widgetPtr->widget );
              lua_pushboolean( L, window->HasStyle( sfg::Window::Style::BACKGROUND ) ? 1 : 0 ); // true/false
              return 1;
            }
          case Tools::Utility::hash( "resize" ):
            {
              if( widgetType != "Window" ) {
                Log::getInstance().warn( "LuaElement::lua_getProperty", "Property \"" + std::string( property ) + "\" does not exist for this widget type." );
                return 0;
              }

              std::shared_ptr< sfg::Window > window = std::static_pointer_cast< sfg::Window >( widgetPtr->widget );
              lua_pushboolean( L, window->HasStyle( sfg::Window::Style::RESIZE ) ? 1 : 0 ); // true/false
              return 1;
            }
          case Tools::Utility::hash( "shadow" ):
            {
              if( widgetType != "Window" ) {
                Log::getInstance().warn( "LuaElement::lua_getProperty", "Property \"" + std::string( property ) + "\" does not exist for this widget type." );
                return 0;
              }

              std::shared_ptr< sfg::Window > window = std::static_pointer_cast< sfg::Window >( widgetPtr->widget );
              lua_pushboolean( L, window->HasStyle( sfg::Window::Style::SHADOW ) ? 1 : 0 ); // true/false
              return 1;
            }
          case Tools::Utility::hash( "close" ):
            {
              if( widgetType != "Window" ) {
                Log::getInstance().warn( "LuaElement::lua_getProperty", "Property \"" + std::string( property ) + "\" does not exist for this widget type." );
                return 0;
              }

              std::shared_ptr< sfg::Window > window = std::static_pointer_cast< sfg::Window >( widgetPtr->widget );
              lua_pushboolean( L, window->HasStyle( sfg::Window::Style::CLOSE ) ? 1 : 0 ); // true/false
              return 1;
            }
          case Tools::Utility::hash( "spacing" ):
            {
              if( widgetType != "Box" ) {
                Log::getInstance().warn( "LuaElement::lua_getProperty", "Property \"" + std::string( property ) + "\" does not exist for this widget type." );
                return 0;
              }

              std::shared_ptr< sfg::Box > box = std::static_pointer_cast< sfg::Box >( widgetPtr->widget );
              lua_pushnumber( L, box->GetSpacing() ); // 42.0
              return 1;
            }
          case Tools::Utility::hash( "alignment_x" ):
            {
              if( widgetType != "Alignment" ) {
                Log::getInstance().warn( "LuaElement::lua_getProperty", "Property \"" + std::string( property ) + "\" does not exist for this widget type." );
                return 0;
              }

              std::shared_ptr< sfg::Alignment > alignmentWidget = std::static_pointer_cast< sfg::Alignment >( widgetPtr->widget );
              sf::Vector2f alignment = alignmentWidget->GetAlignment();
              lua_pushnumber( L, alignment.x ); // 42.0
              return 1;
            }
          case Tools::Utility::hash( "alignment_y" ):
            {
              if( widgetType != "Alignment" ) {
                Log::getInstance().warn( "LuaElement::lua_getProperty", "Property \"" + std::string( property ) + "\" does not exist for this widget type." );
                return 0;
              }

              std::shared_ptr< sfg::Alignment > alignmentWidget = std::static_pointer_cast< sfg::Alignment >( widgetPtr->widget );
              sf::Vector2f alignment = alignmentWidget->GetAlignment();
              lua_pushnumber( L, alignment.y ); // 42.0
              return 1;
            }
          case Tools::Utility::hash( "scale_x" ):
            {
              if( widgetType != "Alignment" ) {
                Log::getInstance().warn( "LuaElement::lua_getProperty", "Property \"" + std::string( property ) + "\" does not exist for this widget type." );
                return 0;
              }

              std::shared_ptr< sfg::Alignment > alignmentWidget = std::static_pointer_cast< sfg::Alignment >( widgetPtr->widget );
              sf::Vector2f scale = alignmentWidget->GetScale();
              lua_pushnumber( L, scale.x ); // 42.0
              return 1;
            }
          case Tools::Utility::hash( "scale_y" ):
            {
              if( widgetType != "Alignment" ) {
                Log::getInstance().warn( "LuaElement::lua_getProperty", "Property \"" + std::string( property ) + "\" does not exist for this widget type." );
                return 0;
              }

              std::shared_ptr< sfg::Alignment > alignmentWidget = std::static_pointer_cast< sfg::Alignment >( widgetPtr->widget );
              sf::Vector2f scale = alignmentWidget->GetScale();
              lua_pushnumber( L, scale.y ); // 42.0
              return 1;
            }
          case Tools::Utility::hash( "value" ):
            {
              if( widgetType == "ProgressBar" ) {
                std::shared_ptr< sfg::ProgressBar > progressBar = std::static_pointer_cast< sfg::ProgressBar >( widgetPtr->widget );
                lua_pushnumber( L, progressBar->GetFraction() ); // 0.5
                return 1;
              } else {
                Log::getInstance().warn( "LuaElement::lua_getProperty", "Property \"" + std::string( property ) + "\" does not exist for this widget type." );
                return 0;
              }
            }
          case Tools::Utility::hash( "scrollable" ):
            if( widgetType == "Notebook" ) {
              std::shared_ptr< sfg::Notebook > notebook = std::static_pointer_cast< sfg::Notebook >( widgetPtr->widget );
              lua_pushboolean( L, notebook->GetScrollable() ); // true
              return 1;
            } else {
              Log::getInstance().warn( "LuaElement::lua_getProperty", "Property \"" + std::string( property ) + "\" does not exist for this widget type." );
              return 0;
            }
          case Tools::Utility::hash( "selected" ):
            if( widgetType == "Notebook" ) {
              std::shared_ptr< sfg::Notebook > notebook = std::static_pointer_cast< sfg::Notebook >( widgetPtr->widget );
              lua_pushnumber( L, notebook->GetCurrentPage() ); // 42
              return 1;
            } else if( widgetType == "ComboBox" ) {
              std::shared_ptr< sfg::ComboBox > comboBox = std::static_pointer_cast< sfg::ComboBox >( widgetPtr->widget );
              lua_pushnumber( L, comboBox->GetSelectedItem() ); // 42
              return 1;
            } else {
              Log::getInstance().warn( "LuaElement::lua_getProperty", "Property \"" + std::string( property ) + "\" does not exist for this widget type." );
              return 0;
            }
          case Tools::Utility::hash( "scrollbar_x" ):
          case Tools::Utility::hash( "scrollbar_y" ):
            if( widgetType == "ScrolledWindow" ) {
              std::shared_ptr< sfg::ScrolledWindow > scrolledWindow = std::static_pointer_cast< sfg::ScrolledWindow >( widgetPtr->widget );
              WidgetBuilder::ScrollbarPolicy policy( scrolledWindow );

              lua_pushstring( L, ( std::string( property ) == "scrollbar_x" ) ? policy.getXAsString().c_str() : policy.getYAsString().c_str() ); // "auto"
              return 1;
            } else {
              Log::getInstance().warn( "LuaElement::lua_getProperty", "Property \"" + std::string( property ) + "\" does not exist for this widget type." );
              return 0;
            }
          case Tools::Utility::hash( "min" ):
          case Tools::Utility::hash( "max" ):
            {
              std::shared_ptr< sfg::Adjustment > adjustment;

              if( widgetType == "Scrollbar" || widgetType == "Scale" ) {
                adjustment = getAdjustment< sfg::Range >( widgetPtr->widget );
              } else if( widgetType == "SpinButton" ) {
                adjustment = getAdjustment< sfg::SpinButton >( widgetPtr->widget );
              } else {
                Log::getInstance().warn( "LuaElement::lua_getProperty", "Property \"" + std::string( property ) + "\" does not exist for this widget type." );
                return 0;
              }

              lua_pushnumber( L, ( std::string( property ) == "min" ? adjustment->GetLower() : adjustment->GetUpper() ) ); // 42
              return 1;
            }
          case Tools::Utility::hash( "minor_step" ):
          case Tools::Utility::hash( "major_step" ):
          case Tools::Utility::hash( "step" ):
            {
              std::shared_ptr< sfg::Adjustment > adjustment;

              if( widgetType == "Scrollbar" || widgetType == "Scale" ) {
                adjustment = getAdjustment< sfg::Range >( widgetPtr->widget );
              } else if ( widgetType == "SpinButton" ) {
                adjustment = getAdjustment< sfg::SpinButton >( widgetPtr->widget );
              } else {
                Log::getInstance().warn( "LuaElement::lua_getProperty", "Property \"" + std::string( property ) + "\" does not exist for this widget type." );
                return 0;
              }

              lua_pushnumber( L, ( ( std::string( property ) == "minor_step" || std::string( property ) == "step" ) ? adjustment->GetMinorStep() : adjustment->GetMajorStep() ) ); // 42
              return 1;
            }
          case Tools::Utility::hash( "precision" ):
            if( widgetType == "SpinButton" ) {
              std::shared_ptr< sfg::SpinButton > spinButton = std::static_pointer_cast< sfg::SpinButton >( widgetPtr->widget );

              lua_pushnumber( L, spinButton->GetDigits() ); // 42.0
              return 1;
            } else {
              Log::getInstance().warn( "LuaElement::lua_getProperty", "Property \"" + std::string( property ) + "\" does not exist for this widget type." );
              return 0;
            }
          case Tools::Utility::hash( "page_size" ):
            if( widgetType == "Scrollbar" || widgetType == "Scale" ) {
              std::shared_ptr< sfg::Range > range = std::static_pointer_cast< sfg::Range >( widgetPtr->widget );
              std::shared_ptr< sfg::Adjustment > adjustment = range->GetAdjustment();

              lua_pushnumber( L, adjustment->GetPageSize() ); // 42
              return 1;
            } else {
              Log::getInstance().warn( "LuaElement::lua_getProperty", "Property \"" + std::string( property ) + "\" does not exist for this widget type." );
              return 0;
            }
          case Tools::Utility::hash( "enabled" ):
            if( widgetType == "ToggleButton" || widgetType == "CheckButton" || widgetType == "RadioButton" ) {
              std::shared_ptr< sfg::ToggleButton > toggleButton = std::static_pointer_cast< sfg::ToggleButton >( widgetPtr->widget );

              lua_pushboolean( L, toggleButton->IsActive() ? 1 : 0 ); // true
              return 1;
            } else {
              Log::getInstance().warn( "LuaElement::lua_getProperty", "Property \"" + std::string( property ) + "\" does not exist for this widget type." );
              return 0;
            }
          // These properties are not settable/retrievable using the SFGUI API
          case Tools::Utility::hash( "tab_position" ):
          case Tools::Utility::hash( "expand" ):
          case Tools::Utility::hash( "fill" ):
          case Tools::Utility::hash( "orientation" ):
          case Tools::Utility::hash( "colspan" ):
          case Tools::Utility::hash( "rowspan" ):
          case Tools::Utility::hash( "padding_x" ):
          case Tools::Utility::hash( "padding_y" ):
          case Tools::Utility::hash( "expand_x" ):
          case Tools::Utility::hash( "expand_y" ):
          case Tools::Utility::hash( "fill_x" ):
          case Tools::Utility::hash( "fill_y" ):
          case Tools::Utility::hash( "row_spacing" ):
          case Tools::Utility::hash( "column_spacing" ):
            {
              Log::getInstance().warn( "LuaElement::lua_getProperty", "Property \"" + std::string( property ) + "\" cannot be queried from this widget." );
              return 0;
            }
          default:
            Log::getInstance().warn( "LuaElement::lua_getProperty", "Property \"" + std::string( property ) + "\" is not a recognized ConcordiaME property." );
            return 0;
        }

        return 0;
      }

      /**
       * @static
       */
      int LuaElement::lua_setProperty( lua_State* L ) {
        LuaElement* widgetPtr = *( ( LuaElement** ) luaL_checkudata( L, 1, "bluebear_widget" ) );

        // ...value "property" self

        char idIndex = 1 - lua_gettop( L );

        if( !lua_isstring( L, idIndex ) ) {
          Log::getInstance().warn( "LuaElement::lua_setProperty", "Argument 1 of set_property must be a string." );
          return 0;
        }

        std::string widgetType = widgetPtr->widget->GetName();

        const char* property = lua_tostring( L, idIndex );
        switch( Tools::Utility::hash( property ) ) {
          case Tools::Utility::hash( "visible" ):
            {
              bool visibility = lua_toboolean( L, -1 ) ? true : false;
              widgetPtr->widget->Show( visibility );
              return 0;
            }
          case Tools::Utility::hash( "id" ):
            {
              if( !lua_isstring( L, -1 ) ) {
                Log::getInstance().warn( "LuaElement::lua_setProperty", "Argument 2 of set_property for property \"id\" must be a string." );
              } else {
                widgetPtr->widget->SetId( lua_tostring( L, -1 ) );
              }
              return 0;
            }
          case Tools::Utility::hash( "class" ):
            {
              if( !lua_isstring( L, -1 ) ) {
                Log::getInstance().warn( "LuaElement::lua_setProperty", "Argument 2 of set_property for property \"class\" must be a string." );
              } else {
                widgetPtr->widget->SetClass( lua_tostring( L, -1 ) );
              }
              return 0;
            }
          case Tools::Utility::hash( "min-width" ):
            {
              // FIXME: Why isn't SFGUI updating window size?

              if( !lua_isnumber( L, -1 ) ) {
                Log::getInstance().warn( "LuaElement::lua_setProperty", "Argument 2 of set_property for property \"min-width\" must be a number." );
                return 0;
              }

              sf::Vector2f requisition = widgetPtr->widget->GetRequisition();
              requisition.x = lua_tonumber( L, -1 );
              widgetPtr->widget->SetRequisition( requisition );
              return 0;
            }
          case Tools::Utility::hash( "min-height" ):
            {
              // FIXME: Why isn't SFGUI updating window size?

              if( !lua_isnumber( L, -1 ) ) {
                Log::getInstance().warn( "LuaElement::lua_setProperty", "Argument 2 of set_property for property \"min-height\" must be a number." );
                return 0;
              }

              sf::Vector2f requisition = widgetPtr->widget->GetRequisition();
              requisition.y = lua_tonumber( L, -1 );
              widgetPtr->widget->SetRequisition( requisition );
              return 0;
            }
          case Tools::Utility::hash( "width" ):
            {
              if( !lua_isnumber( L, -1 ) ) {
                Log::getInstance().warn( "LuaElement::lua_setProperty", "Argument 2 of set_property for property \"width\" must be a number." );
                return 0;
              }

              sf::FloatRect allocation = widgetPtr->widget->GetAllocation();
              allocation.width = lua_tonumber( L, -1 );
              widgetPtr->widget->SetAllocation( allocation );
              return 0;
            }
          case Tools::Utility::hash( "height" ):
            {
              if( !lua_isnumber( L, -1 ) ) {
                Log::getInstance().warn( "LuaElement::lua_setProperty", "Argument 2 of set_property for property \"height\" must be a number." );
                return 0;
              }

              sf::FloatRect allocation = widgetPtr->widget->GetAllocation();
              allocation.height = lua_tonumber( L, -1 );
              widgetPtr->widget->SetAllocation( allocation );
              return 0;
            }
          case Tools::Utility::hash( "left" ):
            {
              if( !lua_isnumber( L, -1 ) ) {
                Log::getInstance().warn( "LuaElement::lua_setProperty", "Argument 2 of set_property for property \"left\" must be a number." );
                return 0;
              }

              sf::FloatRect allocation = widgetPtr->widget->GetAllocation();
              double input = lua_tonumber( L, -1 );
              if( input < 0.0 ) {
                input = ConfigManager::getInstance().getIntValue( "viewport_x" ) + input;
              }

              allocation.left = input;
              widgetPtr->widget->SetAllocation( allocation );
              return 0;
            }
          case Tools::Utility::hash( "top" ):
            {
              if( !lua_isnumber( L, -1 ) ) {
                Log::getInstance().warn( "LuaElement::lua_setProperty", "Argument 2 of set_property for property \"top\" must be a number." );
                return 0;
              }

              sf::FloatRect allocation = widgetPtr->widget->GetAllocation();
              double input = lua_tonumber( L, -1 );
              if( input < 0.0 ) {
                input = ConfigManager::getInstance().getIntValue( "viewport_y" ) + input;
              }

              allocation.top = input;
              widgetPtr->widget->SetAllocation( allocation );
              return 0;
            }
          case Tools::Utility::hash( "title" ):
            {
              if( !lua_isstring( L, -1 ) ) {
                Log::getInstance().warn( "LuaElement::lua_setProperty", "Argument 2 of set_property for property \"title\" must be a string." );
                return 0;
              }

              // Verify this is applicable for the given widget
              if( widgetType == "Window" ) {
                std::shared_ptr< sfg::Window > window = std::static_pointer_cast< sfg::Window >( widgetPtr->widget );
                window->SetTitle( lua_tostring( L, -1 ) );
                return 0;
              } else if( widgetType == "Frame" ) {
                std::shared_ptr< sfg::Frame > frame = std::static_pointer_cast< sfg::Frame >( widgetPtr->widget );
                frame->SetLabel( lua_tostring( L, -1 ) );
                return 0;
              } else {
                Log::getInstance().warn( "LuaElement::lua_setProperty", "Property \"" + std::string( property ) + "\" does not exist for this widget type." );
                return 0;
              }
            }
          case Tools::Utility::hash( "titlebar" ):
            {
              if( widgetType != "Window" ) {
                Log::getInstance().warn( "LuaElement::lua_setProperty", "Property \"" + std::string( property ) + "\" does not exist for this widget type." );
                return 0;
              }

              if( !lua_isboolean( L, -1 ) ) {
                Log::getInstance().warn( "LuaElement::lua_setProperty", "Argument 2 of set_property for property \"titlebar\" must be a boolean." );
                return 0;
              }

              bool titlebar, background, resize, shadow, close;
              std::shared_ptr< sfg::Window > window = std::static_pointer_cast< sfg::Window >( widgetPtr->widget );
              getWindowProps( window, titlebar, background, resize, shadow, close );

              titlebar = lua_toboolean( L, -1 ) ? true : false;

              setWindowProps( window, titlebar, background, resize, shadow, close );
              return 0;
            }
          case Tools::Utility::hash( "background" ):
            {
              if( widgetType != "Window" ) {
                Log::getInstance().warn( "LuaElement::lua_setProperty", "Property \"" + std::string( property ) + "\" does not exist for this widget type." );
                return 0;
              }

              if( !lua_isboolean( L, -1 ) ) {
                Log::getInstance().warn( "LuaElement::lua_setProperty", "Argument 2 of set_property for property \"background\" must be a boolean." );
                return 0;
              }

              bool titlebar, background, resize, shadow, close;
              std::shared_ptr< sfg::Window > window = std::static_pointer_cast< sfg::Window >( widgetPtr->widget );
              getWindowProps( window, titlebar, background, resize, shadow, close );

              background = lua_toboolean( L, -1 ) ? true : false;

              setWindowProps( window, titlebar, background, resize, shadow, close );
              return 0;
            }
          case Tools::Utility::hash( "resize" ):
            {
              if( widgetType != "Window" ) {
                Log::getInstance().warn( "LuaElement::lua_setProperty", "Property \"" + std::string( property ) + "\" does not exist for this widget type." );
                return 0;
              }

              if( !lua_isboolean( L, -1 ) ) {
                Log::getInstance().warn( "LuaElement::lua_setProperty", "Argument 2 of set_property for property \"resize\" must be a boolean." );
                return 0;
              }

              bool titlebar, background, resize, shadow, close;
              std::shared_ptr< sfg::Window > window = std::static_pointer_cast< sfg::Window >( widgetPtr->widget );
              getWindowProps( window, titlebar, background, resize, shadow, close );

              resize = lua_toboolean( L, -1 ) ? true : false;

              setWindowProps( window, titlebar, background, resize, shadow, close );
              return 0;
            }
          case Tools::Utility::hash( "shadow" ):
            {
              if( widgetType != "Window" ) {
                Log::getInstance().warn( "LuaElement::lua_setProperty", "Property \"" + std::string( property ) + "\" does not exist for this widget type." );
                return 0;
              }

              if( !lua_isboolean( L, -1 ) ) {
                Log::getInstance().warn( "LuaElement::lua_setProperty", "Argument 2 of set_property for property \"shadow\" must be a boolean." );
                return 0;
              }

              bool titlebar, background, resize, shadow, close;
              std::shared_ptr< sfg::Window > window = std::static_pointer_cast< sfg::Window >( widgetPtr->widget );
              getWindowProps( window, titlebar, background, resize, shadow, close );

              shadow = lua_toboolean( L, -1 ) ? true : false;

              setWindowProps( window, titlebar, background, resize, shadow, close );
              return 0;
            }
          case Tools::Utility::hash( "close" ):
            {
              if( widgetType != "Window" ) {
                Log::getInstance().warn( "LuaElement::lua_setProperty", "Property \"" + std::string( property ) + "\" does not exist for this widget type." );
                return 0;
              }

              if( !lua_isboolean( L, -1 ) ) {
                Log::getInstance().warn( "LuaElement::lua_setProperty", "Argument 2 of set_property for property \"close\" must be a boolean." );
                return 0;
              }

              bool titlebar, background, resize, shadow, close;
              std::shared_ptr< sfg::Window > window = std::static_pointer_cast< sfg::Window >( widgetPtr->widget );
              getWindowProps( window, titlebar, background, resize, shadow, close );

              close = lua_toboolean( L, -1 ) ? true : false;

              setWindowProps( window, titlebar, background, resize, shadow, close );
              return 0;
            }
          case Tools::Utility::hash( "spacing" ):
            {
              if( widgetType != "Box" ) {
                Log::getInstance().warn( "LuaElement::lua_setProperty", "Property \"" + std::string( property ) + "\" does not exist for this widget type." );
                return 0;
              }

              if( !lua_isnumber( L, -1 ) ) {
                Log::getInstance().warn( "LuaElement::lua_setProperty", "Argument 2 of set_property for property \"spacing\" must be a number." );
                return 0;
              }

              std::shared_ptr< sfg::Box > box = std::static_pointer_cast< sfg::Box >( widgetPtr->widget );
              box->SetSpacing( lua_tonumber( L, -1 ) );
              return 0;
            }
          case Tools::Utility::hash( "alignment_x" ):
            {
              if( widgetType != "Alignment" ) {
                Log::getInstance().warn( "LuaElement::lua_setProperty", "Property \"" + std::string( property ) + "\" does not exist for this widget type." );
                return 0;
              }

              if( !lua_isnumber( L, -1 ) ) {
                Log::getInstance().warn( "LuaElement::lua_setProperty", "Argument 2 of set_property for property \"alignment_x\" must be a number." );
                return 0;
              }

              std::shared_ptr< sfg::Alignment > alignmentWidget = std::static_pointer_cast< sfg::Alignment >( widgetPtr->widget );
              sf::Vector2f alignment = alignmentWidget->GetAlignment();
              alignment.x = lua_tonumber( L, -1 );
              alignmentWidget->SetAlignment( alignment );
              return 0;
            }
          case Tools::Utility::hash( "alignment_y" ):
            {
              if( widgetType != "Alignment" ) {
                Log::getInstance().warn( "LuaElement::lua_setProperty", "Property \"" + std::string( property ) + "\" does not exist for this widget type." );
                return 0;
              }

              if( !lua_isnumber( L, -1 ) ) {
                Log::getInstance().warn( "LuaElement::lua_setProperty", "Argument 2 of set_property for property \"alignment_y\" must be a number." );
                return 0;
              }

              std::shared_ptr< sfg::Alignment > alignmentWidget = std::static_pointer_cast< sfg::Alignment >( widgetPtr->widget );
              sf::Vector2f alignment = alignmentWidget->GetAlignment();
              alignment.y = lua_tonumber( L, -1 );
              alignmentWidget->SetAlignment( alignment );
              return 0;
            }
          case Tools::Utility::hash( "scale_x" ):
            {
              if( widgetType != "Alignment" ) {
                Log::getInstance().warn( "LuaElement::lua_setProperty", "Property \"" + std::string( property ) + "\" does not exist for this widget type." );
                return 0;
              }

              if( !lua_isnumber( L, -1 ) ) {
                Log::getInstance().warn( "LuaElement::lua_setProperty", "Argument 2 of set_property for property \"scale_x\" must be a number." );
                return 0;
              }

              std::shared_ptr< sfg::Alignment > alignmentWidget = std::static_pointer_cast< sfg::Alignment >( widgetPtr->widget );
              sf::Vector2f scale = alignmentWidget->GetScale();
              scale.x = lua_tonumber( L, -1 );
              alignmentWidget->SetScale( scale );
              return 0;
            }
          case Tools::Utility::hash( "scale_y" ):
            {
              if( widgetType != "Alignment" ) {
                Log::getInstance().warn( "LuaElement::lua_setProperty", "Property \"" + std::string( property ) + "\" does not exist for this widget type." );
                return 0;
              }

              if( !lua_isnumber( L, -1 ) ) {
                Log::getInstance().warn( "LuaElement::lua_setProperty", "Argument 2 of set_property for property \"scale_y\" must be a number." );
                return 0;
              }

              std::shared_ptr< sfg::Alignment > alignmentWidget = std::static_pointer_cast< sfg::Alignment >( widgetPtr->widget );
              sf::Vector2f scale = alignmentWidget->GetScale();
              scale.y = lua_tonumber( L, -1 );
              alignmentWidget->SetScale( scale );
              return 0;
            }
          case Tools::Utility::hash( "value" ):
            {
              if( widgetType == "ProgressBar" ) {
                if( !lua_isnumber( L, -1 ) ) {
                  Log::getInstance().warn( "LuaElement::lua_setProperty", "Argument 2 of set_property for property \"value\" must be a number." );
                  return 0;
                }

                std::shared_ptr< sfg::ProgressBar > progressBar = std::static_pointer_cast< sfg::ProgressBar >( widgetPtr->widget );
                progressBar->SetFraction( lua_tonumber( L, -1 ) );
                return 0;
              } else {
                Log::getInstance().warn( "LuaElement::lua_setProperty", "Property \"" + std::string( property ) + "\" does not exist for this widget type." );
                return 0;
              }
            }
          case Tools::Utility::hash( "scrollable" ):
            if( widgetType == "Notebook" ) {
              if( !lua_isboolean( L, -1 ) ) {
                Log::getInstance().warn( "LuaElement::lua_setProperty", "Argument 2 of set_property for property \"" + std::string( property ) + "\" must be a boolean." );
                return 0;
              }

              std::shared_ptr< sfg::Notebook > notebook = std::static_pointer_cast< sfg::Notebook >( widgetPtr->widget );
              notebook->SetScrollable( lua_toboolean( L, -1 ) ? true : false );
              return 0;
            } else {
              Log::getInstance().warn( "LuaElement::lua_setProperty", "Property \"" + std::string( property ) + "\" does not exist for this widget type." );
              return 0;
            }
          case Tools::Utility::hash( "selected" ):
            if( !lua_isnumber( L, -1 ) ) {
              Log::getInstance().warn( "LuaElement::lua_setProperty", "Argument 2 of set_property for property \"" + std::string( property ) + "\" must be a number." );
              return 0;
            }

            if( widgetType == "Notebook" ) {
              std::shared_ptr< sfg::Notebook > notebook = std::static_pointer_cast< sfg::Notebook >( widgetPtr->widget );
              notebook->SetCurrentPage( lua_tonumber( L, -1 ) );
              return 0;
            } else if( widgetType == "ComboBox" ) {
              std::shared_ptr< sfg::ComboBox > comboBox = std::static_pointer_cast< sfg::ComboBox >( widgetPtr->widget );
              comboBox->SelectItem( lua_tonumber( L, -1 ) );
              return 0;
            } else {
              Log::getInstance().warn( "LuaElement::lua_setProperty", "Property \"" + std::string( property ) + "\" does not exist for this widget type." );
              return 0;
            }
          case Tools::Utility::hash( "scrollbar_x" ):
          case Tools::Utility::hash( "scrollbar_y" ):
            if( widgetType == "ScrolledWindow" ) {
              if( !lua_isstring( L, -1 ) ) {
                Log::getInstance().warn( "LuaElement::lua_setProperty", "Argument 2 of set_property for property \"" + std::string( property ) + "\" must be a string." );
                return 0;
              }

              std::shared_ptr< sfg::ScrolledWindow > scrolledWindow = std::static_pointer_cast< sfg::ScrolledWindow >( widgetPtr->widget );
              WidgetBuilder::ScrollbarPolicy policy( scrolledWindow );
              if( std::string( property ) == "scrollbar_x" ) {
                // scrollbar_x
                policy.setX( lua_tostring( L, -1 ) );
              } else {
                // scrollbar_y
                policy.setY( lua_tostring( L, -1 ) );
              }

              scrolledWindow->SetScrollbarPolicy( policy.get() );

              return 0;
            } else {
              Log::getInstance().warn( "LuaElement::lua_getProperty", "Property \"" + std::string( property ) + "\" does not exist for this widget type." );
              return 0;
            }
          case Tools::Utility::hash( "row_spacing" ):
          case Tools::Utility::hash( "column_spacing" ):
            {
              // This should be one of very few set_property properties that take one or two values
              char args = lua_gettop( L );

              if( args == 4 ) {
                // value cell "spacing" self
                if( !lua_isnumber( L, -1 ) || !lua_isnumber( L, -2 ) ) {
                  Log::getInstance().warn( "LuaElement::lua_setProperty", "Arguments 2 and 3 of set_property for property \"" + std::string( property ) + "\" must be numbers." );
                  return 0;
                }

                float spacing = lua_tonumber( L, -1 );
                int item = lua_tonumber( L, -2 );

                std::shared_ptr< sfg::Table > table = std::static_pointer_cast< sfg::Table >( widgetPtr->widget );
                if( std::string( property ) == "row_spacing" ) {
                  // row spacing
                  table->SetRowSpacing( item, spacing );
                } else {
                  // column spacing
                  table->SetColumnSpacing( item, spacing );
                }
              } else {
                // value "spacing" self
                if( !lua_isnumber( L, -1 ) ) {
                  Log::getInstance().warn( "LuaElement::lua_setProperty", "Argument 2 of set_property for property \"" + std::string( property ) + "\" must be a number." );
                  return 0;
                }

                float spacing = lua_tonumber( L, -1 );
                std::shared_ptr< sfg::Table > table = std::static_pointer_cast< sfg::Table >( widgetPtr->widget );
                if( std::string( property ) == "row_spacing" ) {
                  // row spacing
                  table->SetRowSpacings( spacing );
                } else {
                  // column spacing
                  table->SetColumnSpacings( spacing );
                }
              }
              return 0;
            }
          case Tools::Utility::hash( "min" ):
          case Tools::Utility::hash( "max" ):
            {
              if( !lua_isnumber( L, -1 ) ) {
                Log::getInstance().warn( "LuaElement::lua_setProperty", "Argument 2 of set_property for property \"" + std::string( property ) + "\" must be a number." );
                return 0;
              }

              std::shared_ptr< sfg::Adjustment > adjustment;

              if( widgetType == "Scrollbar" || widgetType == "Scale" ) {
                adjustment = getAdjustment< sfg::Range >( widgetPtr->widget );
              } else if( widgetType == "SpinButton" ) {
                adjustment = getAdjustment< sfg::SpinButton >( widgetPtr->widget );
              } else {
                Log::getInstance().warn( "LuaElement::lua_setProperty", "Property \"" + std::string( property ) + "\" does not exist for this widget type." );
                return 0;
              }

              if( std::string( property ) == "min" ) {
                adjustment->SetLower( lua_tonumber( L, -1 ) );
              } else {
                adjustment->SetUpper( lua_tonumber( L, -1 ) );
              }

              return 0;
            }
          case Tools::Utility::hash( "minor_step" ):
          case Tools::Utility::hash( "major_step" ):
          case Tools::Utility::hash( "step" ):
            {
              if( !lua_isnumber( L, -1 ) ) {
                Log::getInstance().warn( "LuaElement::lua_setProperty", "Argument 2 of set_property for property \"" + std::string( property ) + "\" must be a number." );
                return 0;
              }

              std::shared_ptr< sfg::Adjustment > adjustment;

              if( widgetType == "Scrollbar" || widgetType == "Scale" ) {
                adjustment = getAdjustment< sfg::Range >( widgetPtr->widget );
              } else if( widgetType == "SpinButton" ) {
                adjustment = getAdjustment< sfg::SpinButton >( widgetPtr->widget );
              } else {
                Log::getInstance().warn( "LuaElement::lua_setProperty", "Property \"" + std::string( property ) + "\" does not exist for this widget type." );
                return 0;
              }

              if( std::string( property ) == "minor_step" || std::string( property ) == "step" ) {
                adjustment->SetMinorStep( lua_tonumber( L, -1 ) );
              } else {
                adjustment->SetMajorStep( lua_tonumber( L, -1 ) );
              }

              return 0;
            }
          case Tools::Utility::hash( "precision" ):
            if( widgetType == "SpinButton" ) {
              if( !lua_isnumber( L, -1 ) ) {
                Log::getInstance().warn( "LuaElement::lua_setProperty", "Argument 2 of set_property for property \"" + std::string( property ) + "\" must be a number." );
                return 0;
              }

              std::shared_ptr< sfg::SpinButton > spinButton = std::static_pointer_cast< sfg::SpinButton >( widgetPtr->widget );
              spinButton->SetDigits( lua_tonumber( L, -1 ) );
              return 0;
            } else {
              Log::getInstance().warn( "LuaElement::lua_setProperty", "Property \"" + std::string( property ) + "\" does not exist for this widget type." );
              return 0;
            }
          case Tools::Utility::hash( "page_size" ):
            if( widgetType == "Scrollbar" || widgetType == "Scale" ) {
              if( !lua_isnumber( L, -1 ) ) {
                Log::getInstance().warn( "LuaElement::lua_setProperty", "Argument 2 of set_property for property \"" + std::string( property ) + "\" must be a number." );
                return 0;
              }

              std::shared_ptr< sfg::Range > range = std::static_pointer_cast< sfg::Range >( widgetPtr->widget );
              std::shared_ptr< sfg::Adjustment > adjustment = range->GetAdjustment();

              adjustment->SetPageSize( lua_tonumber( L, -1 ) );

              return 0;
            } else {
              Log::getInstance().warn( "LuaElement::lua_setProperty", "Property \"" + std::string( property ) + "\" does not exist for this widget type." );
              return 0;
            }
          case Tools::Utility::hash( "enabled" ):
            if( widgetType == "ToggleButton" || widgetType == "CheckButton" || widgetType == "RadioButton" ) {
              if( !lua_isboolean( L, -1 ) ) {
                Log::getInstance().warn( "LuaElement::lua_setProperty", "Argument 2 of set_property for property \"" + std::string( property ) + "\" must be a boolean." );
                return 0;
              }

              std::shared_ptr< sfg::ToggleButton > toggleButton = std::static_pointer_cast< sfg::ToggleButton >( widgetPtr->widget );
              toggleButton->SetActive( lua_toboolean( L, -1 ) ? true : false );
              return 0;
            } else {
              Log::getInstance().warn( "LuaElement::lua_setProperty", "Property \"" + std::string( property ) + "\" does not exist for this widget type." );
              return 0;
            }
          // These properties are not settable/retrievable using the SFGUI API
          case Tools::Utility::hash( "tab_position" ):
            // Tried to make tab_position settable.
            // There appears to be an SFGUI bug here where if we set the tab_position on an existing Notebook, it just all goes to hell and will not render properly.
          case Tools::Utility::hash( "expand" ):
          case Tools::Utility::hash( "fill" ):
          case Tools::Utility::hash( "orientation" ):
            {
              Log::getInstance().warn( "LuaElement::lua_setProperty", "Property \"" + std::string( property ) + "\" cannot be set on this widget." );
              return 0;
            }
          default:
            Log::getInstance().warn( "LuaElement::lua_setProperty", "Property \"" + std::string( property ) + "\" is not a recognized ConcordiaME property." );
        }

        return 0;
      }

      /**
       * @static
       */
      int LuaElement::lua_gc( lua_State* L ) {
        LuaElement* widgetPtr = *( ( LuaElement** ) luaL_checkudata( L, 1, "bluebear_widget" ) );

        // Destroy the std::shared_ptr< sfg::Widget >. This should decrease the reference count by one.
        //Log::getInstance().debug( "LuaElement::lua_gc", "Deleting " + Tools::Utility::pointerToString( widgetPtr ) );
        delete widgetPtr;

        // Cycle GC for master maps
        masterMapGC();

        return 0;
      }

      /**
       * @static
       */
      int LuaElement::lua_add( lua_State* L ) {
        LuaElement* self = *( ( LuaElement** ) luaL_checkudata( L, 1, "bluebear_widget" ) );

        if( lua_isstring( L, -1 ) ) {
          Display::MainGameState* state = ( Display::MainGameState* )lua_touserdata( L, lua_upvalueindex( 1 ) );
          self->add( L, lua_tostring( L, -1 ), *state );
        } else if ( PagePseudoElement** udata = ( PagePseudoElement** ) luaL_testudata( L, 2, "bluebear_page_pseudo_element" ) ) {
          self->add( L, *udata );
        } else if ( ItemPseudoElement** udata = ( ItemPseudoElement** ) luaL_testudata( L, 2, "bluebear_item_pseudo_element" ) ) {
          self->add( L, *udata );
        } else {
          LuaElement* argument = *( ( LuaElement** ) luaL_checkudata( L, 2, "bluebear_widget" ) );
          self->add( argument );
        }

        return 0;
      }

      int LuaElement::lua_widgetRemove( lua_State* L ) {
        VERIFY_USER_DATA( "LuaElement::lua_widgetRemove", "remove" );

        LuaElement* self = *( ( LuaElement** ) luaL_checkudata( L, 1, "bluebear_widget" ) );

        if( LuaElement** udata = ( LuaElement** ) luaL_testudata( L, 2, "bluebear_widget" ) ) {
          // "argument" should be removed from "self" if and only if "self" is a container
          self->removeWidget( ( *udata )->widget );
        } else if( PagePseudoElement** udata = ( PagePseudoElement** ) luaL_testudata( L, 2, "bluebear_page_pseudo_element" ) ) {
          self->removePseudoElement( *udata );
        } else {
          Log::getInstance().warn( "LuaElement::lua_widgetRemove", "Invalid argument passed to remove()" );
        }

        return 0;
      }

      /**
       * @static
       */
      void LuaElement::getWindowProps( std::shared_ptr< sfg::Window > window, bool& titlebar, bool& background, bool& resize, bool& shadow, bool& close ) {
        titlebar = window->HasStyle( sfg::Window::Style::TITLEBAR );
        background = window->HasStyle( sfg::Window::Style::BACKGROUND );
        resize = window->HasStyle( sfg::Window::Style::RESIZE );
        shadow = window->HasStyle( sfg::Window::Style::SHADOW );
        close = window->HasStyle( sfg::Window::Style::CLOSE );
      }

      /**
       * @static
       */
      void LuaElement::setWindowProps( std::shared_ptr< sfg::Window > window, bool titlebar, bool background, bool resize, bool shadow, bool close ) {
        window->SetStyle(
          ( titlebar ? 1 : 0 ) |
          ( ( background ? 1 : 0 ) << 1 ) |
          ( ( resize ? 1 : 0 ) << 2 ) |
          ( ( shadow ? 1 : 0 ) << 3 ) |
          ( ( close ? 1 : 0 ) << 4 )
        );
      }

      /**
       *
       * STACK ARGS: (none)
       * RETURNS: userdata
       */
      void LuaElement::getUserdataFromWidget( lua_State* L, std::shared_ptr< sfg::Widget > widget ) {
        LuaElement** userData = ( LuaElement** )lua_newuserdata( L, sizeof( LuaElement* ) ); // userdata
        *userData = new LuaElement();

        ( **userData ).widget = widget;

        luaL_getmetatable( L, "bluebear_widget" ); // metatable userdata
        lua_setmetatable( L, -2 ); // userdata
      }

      /**
       *
       * STACK ARGS: (none)
       * Stack is unmodified after call
       */
      void LuaElement::clickHandler( lua_State* L, EventManager& eventManager, std::weak_ptr< sfg::Widget > selfElement, LuaReference masterReference, const std::string& buttonTag ) {
        // Create new "disposable" reference that will get ferried through and double-bag it with an event meta object

        // Double-bag this function by slapping an event object onto the argument list
        lua_getglobal( L, "bluebear" ); // bluebear
        Tools::Utility::getTableValue( L, "util" ); // bluebear.util bluebear
        Tools::Utility::getTableValue( L, "bind" ); // <bind> bluebear.util bluebear
        lua_rawgeti( L, LUA_REGISTRYINDEX, masterReference ); // <function> <bind> bluebear.util bluebear

        lua_newtable( L ); // newtable <function> <bind> bluebear.util bluebear
        lua_pushstring( L, "mouse" ); // "mouse" newtable <function> <bind> bluebear.util bluebear
        lua_pushstring( L, buttonTag.c_str() ); // "left" "mouse" newtable <function> <bind> bluebear.util bluebear
        lua_settable( L, -3 ); // newtable <function> <bind> bluebear.util bluebear

        setKeyboardStatus( L );

        if( auto elementPtr = selfElement.lock() ) {
          lua_pushstring( L, "widget" ); // "widget" newtable <function> <bind> bluebear.util bluebear
          getUserdataFromWidget( L, elementPtr ); // element "widget" newtable <function> <bind> bluebear.util bluebear
          lua_settable( L, -3 ); // newtable <function> <bind> bluebear.util bluebear
        } else {
          Log::getInstance().error( "LuaElement::clickHandler", "Could not lock element pointer to build field event.widget" );
        }

        if( lua_pcall( L, 2, 1, 0 ) ) { // error bluebear.util bluebear
          Log::getInstance().error( "LuaElement::clickHandler", "Couldn't create required closure to fire event." );
          lua_pop( L, 3 ); // EMPTY
          return;
        } // <temp_function> bluebear.util bluebear

        int edibleReference = luaL_ref( L, LUA_REGISTRYINDEX ); // bluebear.util bluebear
        lua_pop( L, 2 ); // EMPTY

        eventManager.UI_ACTION_EVENT.trigger( edibleReference );
      }

      /**
       * @static
       * Generic event handler for most events that passes a baseline event object, double-bags the function, and calls it
       *
       * STACK ARGS: (none)
       * Stack is unmodified after call
       */
      void LuaElement::genericHandler( lua_State* L, EventManager& eventManager, std::weak_ptr< sfg::Widget > widgetPtr, LuaReference masterReference ) {
        lua_getglobal( L, "bluebear" ); // bluebear
        Tools::Utility::getTableValue( L, "util" ); // bluebear.util bluebear
        Tools::Utility::getTableValue( L, "bind" ); // <bind> bluebear.util bluebear
        lua_rawgeti( L, LUA_REGISTRYINDEX, masterReference ); // <function> <bind> bluebear.util bluebear

        lua_newtable( L ); // newtable <function> <bind> bluebear.util bluebear

        setKeyboardStatus( L );

        if( auto widget = widgetPtr.lock() ) {
          lua_pushstring( L, "widget" ); // "widget" newtable <function> <bind> bluebear.util bluebear
          getUserdataFromWidget( L, widget ); // element "widget" newtable <function> <bind> bluebear.util bluebear
          lua_settable( L, -3 ); // newtable <function> <bind> bluebear.util bluebear
        } else {
          Log::getInstance().error( "LuaElement::genericHandler", "Could not lock element pointer to build field event.widget" );
        }

        if( lua_pcall( L, 2, 1, 0 ) ) { // error bluebear.util bluebear
          Log::getInstance().error( "LuaElement::genericHandler", "Couldn't create required closure to fire event." );
          lua_pop( L, 3 ); // EMPTY
          return;
        } // <temp_function> bluebear.util bluebear

        int edibleReference = luaL_ref( L, LUA_REGISTRYINDEX ); // bluebear.util bluebear
        lua_pop( L, 2 ); // EMPTY

        eventManager.UI_ACTION_EVENT.trigger( edibleReference );
      }

      /**
       *
       * STACK ARGS: newtable
       * (Stack is unmodified after call)
       */
      void LuaElement::setKeyboardStatus( lua_State* L ) {
        lua_pushstring( L, "keyboard" ); // "keyboard" newtable
        lua_newtable( L ); // keyboard "keyboard" newtable

        lua_pushstring( L, "ctrl" ); // "ctrl" keyboard "keyboard" newtable
        lua_pushboolean( L, ( sf::Keyboard::isKeyPressed( sf::Keyboard::LControl ) || sf::Keyboard::isKeyPressed( sf::Keyboard::RControl ) ) ? 1 : 0 );  // true "ctrl" keyboard "keyboard" newtable
        lua_settable( L, -3 ); // keyboard "keyboard" newtable

        lua_pushstring( L, "alt" ); // "alt" keyboard "keyboard" newtable
        lua_pushboolean( L, ( sf::Keyboard::isKeyPressed( sf::Keyboard::LAlt ) || sf::Keyboard::isKeyPressed( sf::Keyboard::RAlt ) ) ? 1 : 0 );  // true "alt" keyboard "keyboard" newtable
        lua_settable( L, -3 ); // keyboard "keyboard" newtable

        lua_pushstring( L, "meta" ); // "meta" keyboard "keyboard" newtable
        lua_pushboolean( L, ( sf::Keyboard::isKeyPressed( sf::Keyboard::LSystem ) || sf::Keyboard::isKeyPressed( sf::Keyboard::RSystem ) ) ? 1 : 0 );  // true "meta" keyboard "keyboard" newtable
        lua_settable( L, -3 ); // keyboard "keyboard" newtable

        lua_settable( L, -3 ); // newtable
      }

      /**
       * @static
       * Unregister a click handler registered by a LuaElement
       */
      void LuaElement::unregisterClickHandler( lua_State* L, std::map< sfg::Signal::SignalID, LuaElement::SignalBinding >& signalMap, std::shared_ptr< sfg::Widget > widget ) {
        // If there's a previous pair of click events registered for this widget instance, unref and kill it
        auto pair = signalMap.find( sfg::Widget::OnLeftClick );
        if( pair != signalMap.end() ) {
          // One click listener at a time
          // Disconnect the handler for left click
          widget->GetSignal( sfg::Widget::OnLeftClick ).Disconnect( pair->second.slotHandle );
          // This is a package deal, do it for the right click handler as well
          widget->GetSignal( sfg::Widget::OnRightClick ).Disconnect( signalMap.at( sfg::Widget::OnRightClick ).slotHandle );

          // Un-ref this function we're about to erase (left and right should use the same reference)
          luaL_unref( L, LUA_REGISTRYINDEX, pair->second.reference );

          signalMap.erase( sfg::Widget::OnLeftClick );
          signalMap.erase( sfg::Widget::OnRightClick );
        }
      }

      /**
       * @static
       */
      void LuaElement::unregisterHandler( lua_State* L, std::map< sfg::Signal::SignalID, LuaElement::SignalBinding >& signalMap, std::shared_ptr< sfg::Widget > widget, sfg::Signal::SignalID signalID ) {
        auto pair = signalMap.find( signalID );
        if( pair != signalMap.end() ) {
          widget->GetSignal( signalID ).Disconnect( pair->second.slotHandle );
          luaL_unref( L, LUA_REGISTRYINDEX, pair->second.reference );
        }
      }

      /**
       * @static
       *
       * STACK ARGS: function
       * RETURNS: true
       */
      void LuaElement::registerGenericHandler( lua_State* L, EventManager& eventManager, std::shared_ptr< sfg::Widget > widget, sfg::Signal::SignalID signalID ) {
        auto& signalMap = masterSignalMap[ widget ];
        unregisterHandler( L, signalMap, widget, signalID );

        LuaReference masterReference = luaL_ref( L, LUA_REGISTRYINDEX ); // EMPTY

        signalMap[ signalID ] = LuaElement::SignalBinding{
          masterReference,
          widget->GetSignal( signalID ).Connect( std::bind( LuaElement::genericHandler, L, eventManager, widget, masterReference ) )
        };

        lua_pushboolean( L, true ); // true
      }

      void LuaElement::setCustomAttribute( std::shared_ptr< sfg::Widget > widget, const std::string& key, const std::string& value ) {
        // If we're unsetting a particular attribute and there's no map for this widget, just ignore it
        if( value == "" && masterAttrMap.find( widget ) == masterAttrMap.end() ) {
          return;
        }

        std::map< std::string, std::string > attrMap = masterAttrMap[ widget ];
        if( value == "" ) {
          // Unsetting an existing map value
          attrMap.erase( key );

          // If this was the last value, clean up the map
          if( !attrMap.size() ) {
            masterAttrMap.erase( widget );
          }
        } else {
          // Setting a new map value
          attrMap[ key ] = value;
        }
      }

      std::string LuaElement::getCustomAttribute( std::shared_ptr< sfg::Widget > widget, const std::string& key ) {
        auto it = masterAttrMap.find( widget );

        if( it != masterAttrMap.end() ) {
          std::map< std::string, std::string > attrMap = it->second;
          auto it = attrMap.find( key );
          if( it != attrMap.end() ) {
            return it->second;
          }
        }

        return "";
      }

      void LuaElement::queryUnsignedAttribute( std::shared_ptr< sfg::Widget > widget, const std::string& key, unsigned int* value ) {
        std::string stringValue = getCustomAttribute( widget, key );

        if( stringValue != "" ) {
          *value = std::stoul( stringValue );
        }
      }

      void LuaElement::queryFloatAttribute( std::shared_ptr< sfg::Widget > widget, const std::string& key, float* value ) {
        std::string stringValue = getCustomAttribute( widget, key );

        if( stringValue != "" ) {
          *value = std::stof( stringValue );
        }
      }

      void LuaElement::queryBoolAttribute( std::shared_ptr< sfg::Widget > widget, const std::string& key, bool* value ) {
        std::string stringValue = getCustomAttribute( widget, key );

        if( stringValue != "" ) {
          // non-false, non-blank values are truthy
          *value = !( stringValue == "false" );
        }
      }

      /**
       * Garbage-collect potentially abandoned items that masterSignalMap/masterAttrMap are keeping alive
       */
      void LuaElement::masterMapGC() {
        {
          auto it = masterSignalMap.begin();
          while( it != masterSignalMap.end() ) {

            if( it->first.use_count() == 1 ) {
              it = masterSignalMap.erase( it );
            } else {
              ++it;
            }

          }
        }

        {
          auto it = masterAttrMap.begin();
          while( it != masterAttrMap.end() ) {

            if( it->first.use_count() == 1 ) {
              it = masterAttrMap.erase( it );
            } else {
              ++it;
            }

          }
        }
      }

    }
  }
}
