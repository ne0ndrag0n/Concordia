#ifndef WIDGETBUILDER
#define WIDGETBUILDER

#include <tinyxml2.h>
#include <SFGUI/Widget.hpp>
#include <SFGUI/Window.hpp>
#include <SFGUI/Label.hpp>
#include <SFGUI/Container.hpp>
#include <SFGUI/Box.hpp>
#include <SFGUI/Alignment.hpp>
#include <SFGUI/Misc.hpp>
#include <SFGUI/Button.hpp>
#include <SFGUI/Entry.hpp>
#include <SFGUI/Image.hpp>
#include <SFGUI/Frame.hpp>
#include <SFGUI/ProgressBar.hpp>
#include <SFGUI/Separator.hpp>
#include <SFGUI/Notebook.hpp>
#include <string>
#include <memory>
#include <exception>
#include <vector>
#include "log.hpp"
#include "tools/utility.hpp"

namespace BlueBear {
  class EventManager;

  namespace Graphics {
    class ImageCache;

    class WidgetBuilder {
      EventManager& eventManager;
      ImageCache& imageCache;
      tinyxml2::XMLDocument document;
      static constexpr unsigned int hash(const char* str, int h = 0);

      void addChildren( std::shared_ptr< sfg::Container > widget, tinyxml2::XMLElement* element );
      void packChildren( std::shared_ptr< sfg::Box > widget, tinyxml2::XMLElement* element );
      void addNotebookTabs( std::shared_ptr< sfg::Notebook > notebook, tinyxml2::XMLElement* element );

      std::shared_ptr< sfg::Window > newWindowWidget( tinyxml2::XMLElement* element );
      std::shared_ptr< sfg::Label > newLabelWidget( tinyxml2::XMLElement* element );
      std::shared_ptr< sfg::Box > newBoxWidget( tinyxml2::XMLElement* element );
      std::shared_ptr< sfg::Alignment > newAlignmentWidget( tinyxml2::XMLElement* element );
      std::shared_ptr< sfg::Button > newButtonWidget( tinyxml2::XMLElement* element );
      std::shared_ptr< sfg::Entry > newEntryWidget( tinyxml2::XMLElement* element );
      std::shared_ptr< sfg::Image > newImageWidget( tinyxml2::XMLElement* element );
      std::shared_ptr< sfg::Frame > newFrameWidget( tinyxml2::XMLElement* element );
      std::shared_ptr< sfg::ProgressBar > newProgressBarWidget( tinyxml2::XMLElement* element );
      std::shared_ptr< sfg::Separator > newSeparatorWidget( tinyxml2::XMLElement* element );
      std::shared_ptr< sfg::Notebook > newNotebookWidget( tinyxml2::XMLElement* element );

      void setDefaultEvents( std::shared_ptr< sfg::Widget > widget, tinyxml2::XMLElement* element );
      void setAlignment( std::shared_ptr< sfg::Misc > widget, tinyxml2::XMLElement* element );
      void setBasicProperties( std::shared_ptr< sfg::Widget > widget, tinyxml2::XMLElement* element );
      void setAllocationAndRequisition( std::shared_ptr< sfg::Widget > widget, tinyxml2::XMLElement* element );

      std::shared_ptr< sfg::Widget > nodeToWidget( tinyxml2::XMLElement* element );

      void correctXBoundary( float* input );
      void correctYBoundary( float* input );

    public:
      WidgetBuilder( EventManager& eventManager, ImageCache& imageCache, const std::string& path );
      std::vector< std::shared_ptr< sfg::Widget > > getWidgets();

      template< typename T > static T getOrientation( const char* orientation ) {
        if( !orientation ) {
          orientation = "horizontal";
        }

        T orientationFlag;

        switch( Tools::Utility::hash( orientation ) ) {
          case Tools::Utility::hash( "vertical" ):
            orientationFlag = T::VERTICAL;
            break;
          default:
            Log::getInstance().warn( "WidgetBuilder::getOrientation", "Invalid value for \"orientation\" attribute: " + std::string( orientation ) + ", defaulting to \"horizontal\"" );
          case Tools::Utility::hash( "horizontal" ):
            orientationFlag = T::HORIZONTAL;
            break;
        }

        return orientationFlag;
      };

      template< typename T > static T getPosition( const char* tabs ) {
        if( !tabs ) {
          tabs = "top";
        }

        T tabPosition;
        switch( Tools::Utility::hash( tabs ) ) {
          default:
            Log::getInstance().warn( "WidgetBuilder::getPosition", "Invalid value for \"tab_position\" attribute: " + std::string( tabs ) + ", defaulting to \"top\"" );
          case Tools::Utility::hash( "top" ):
            tabPosition = T::TOP;
            break;
          case Tools::Utility::hash( "bottom" ):
            tabPosition = T::BOTTOM;
            break;
          case Tools::Utility::hash( "left" ):
            tabPosition = T::LEFT;
            break;
          case Tools::Utility::hash( "right" ):
            tabPosition = T::RIGHT;
        }

        return tabPosition;
      };

      template< typename T > static std::string getPosition( T position ) {
        switch( position ) {
          default:
          case T::TOP:
            return "top";
          case T::BOTTOM:
            return "bottom";
          case T::LEFT:
            return "left";
          case T::RIGHT:
            return "right";
        }
      };
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
