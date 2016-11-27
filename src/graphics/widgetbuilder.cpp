#include "graphics/widgetbuilder.hpp"
#include "log.hpp"
#include <string>
#include <memory>
#include <SFGUI/Widget.hpp>
#include <SFGUI/Window.hpp>
#include <SFGUI/Container.hpp>
#include <SFGUI/Label.hpp>
#include <tinyxml2.h>
#include <vector>

namespace BlueBear {
  namespace Graphics {

    WidgetBuilder::WidgetBuilder( const std::string& path ) {
      document.LoadFile( path.c_str() );

      // Object is unusable if the file failed to load
      if( document.ErrorID() ) {
        throw FailedToLoadXMLException();
      }
    }

    constexpr unsigned int WidgetBuilder::hash(const char* str, int h) {
      return !str[h] ? 5381 : (hash(str, h+1) * 33) ^ str[h];
    }

    /**
     * This is where it all happens. Call this method to turn your XML file into a collection of widgets.
     */
    std::vector< std::shared_ptr< sfg::Widget > > WidgetBuilder::getWidgets() {
      std::vector< std::shared_ptr< sfg::Widget > > widgets;

      for ( tinyxml2::XMLElement* node = document.RootElement(); node != NULL; node = node->NextSiblingElement() ) {
        widgets.push_back( nodeToWidget( node ) );
      }

      return widgets;
    }

    std::shared_ptr< sfg::Widget > WidgetBuilder::nodeToWidget( tinyxml2::XMLElement* element ) {
      std::shared_ptr< sfg::Widget > widget;

      const char* tagType = element->Name();

      switch( hash( tagType ) ) {

        case hash( "Window" ):
          widget = newWindowWidget( element );
          addChildren( std::static_pointer_cast< sfg::Container >( widget ), element );
          break;

        case hash( "Label" ):
          widget = newLabelWidget( element );
          break;

        default:
          Log::getInstance().error( "WidgetBuilder::nodeToWidget", "Invalid CME tag specified: " + std::string( tagType ) );
          throw InvalidCMEWidgetException();
      }

      return widget;
    }

    void WidgetBuilder::addChildren( std::shared_ptr< sfg::Container > widget, tinyxml2::XMLElement* element ) {
      for ( tinyxml2::XMLElement* child = element->FirstChildElement(); child != NULL; child = child->NextSiblingElement() ) {
        widget->Add( nodeToWidget( child ) );
      }
    }

    void WidgetBuilder::setIdAndClass( std::shared_ptr< sfg::Widget > widget, tinyxml2::XMLElement* element ) {
      const char* id = element->Attribute( "id" );
      const char* clss = element->Attribute( "class" );

      if ( id ) {
        widget->SetId( id );
      }

      if ( clss ) {
        widget->SetClass( clss );
      }
    }

    std::shared_ptr< sfg::Window > WidgetBuilder::newWindowWidget( tinyxml2::XMLElement* element ) {
      std::shared_ptr< sfg::Window > window = sfg::Window::Create();

      setIdAndClass( window, element );

      const char* title = element->Attribute( "title" );
      if( title ) {
        window->SetTitle( title );
      }

      // number |= 1 << x; to set a bit
      // number &= ~(1 << x); to clear a bit
      // (ID >> position) & 1 to get specific bit

      // Unpack these properties
      bool titlebar = window->HasStyle( sfg::Window::Style::TITLEBAR );
      bool background = window->HasStyle( sfg::Window::Style::BACKGROUND );
      bool resize = window->HasStyle( sfg::Window::Style::RESIZE );
      bool shadow = window->HasStyle( sfg::Window::Style::SHADOW );
      bool close = window->HasStyle( sfg::Window::Style::CLOSE );

      element->QueryBoolAttribute( "titlebar", &titlebar );
      element->QueryBoolAttribute( "background", &background );
      element->QueryBoolAttribute( "resize", &resize );
      element->QueryBoolAttribute( "shadow", &shadow );
      element->QueryBoolAttribute( "close", &close );

      window->SetStyle(
        ( titlebar ? 1 : 0 ) |
        ( ( background ? 1 : 0 ) << 1 ) |
        ( ( resize ? 1 : 0 ) << 2 ) |
        ( ( shadow ? 1 : 0 ) << 3 ) |
        ( ( close ? 1 : 0 ) << 4 )
      );

      return window;
    }

    std::shared_ptr< sfg::Label > WidgetBuilder::newLabelWidget( tinyxml2::XMLElement* element ) {
      std::shared_ptr< sfg::Label > label;

      const char* labelValue = element->GetText();

      if( labelValue ) {
        label = sfg::Label::Create( labelValue );
      } else {
        label = sfg::Label::Create( "" );
      }

      return label;
    }

  }
}
