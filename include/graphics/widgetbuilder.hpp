#ifndef WIDGETBUILDER
#define WIDGETBUILDER

#include <tinyxml2.h>
#include <SFGUI/Widget.hpp>
#include <SFGUI/Window.hpp>
#include <SFGUI/Label.hpp>
#include <SFGUI/Container.hpp>
#include <string>
#include <memory>
#include <exception>
#include <vector>

namespace BlueBear {
  namespace Graphics {

    class WidgetBuilder {
      tinyxml2::XMLDocument document;
      static constexpr unsigned int hash(const char* str, int h = 0);

      void addChildren( std::shared_ptr< sfg::Container > widget, tinyxml2::XMLElement* element );
      std::shared_ptr< sfg::Window > newWindowWidget( tinyxml2::XMLElement* element );
      std::shared_ptr< sfg::Label > newLabelWidget( tinyxml2::XMLElement* element );
      void setIdAndClass( std::shared_ptr< sfg::Widget > widget, tinyxml2::XMLElement* element );
      std::shared_ptr< sfg::Widget > nodeToWidget( tinyxml2::XMLElement* element );

    public:
      WidgetBuilder( const std::string& path );
      std::vector< std::shared_ptr< sfg::Widget > > getWidgets();
    };

    struct InvalidCMEWidgetException : public std::exception {
      const char* what() const throw() { return "Invalid tag type specified!"; }
    };

    struct FailedToLoadXMLException : public std::exception {
      const char* what() const throw() { return "Error loading XML!"; }
    };

  }
}

#endif
