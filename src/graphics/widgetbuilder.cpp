#include "graphics/widgetbuilder.hpp"
#include "graphics/imagecache.hpp"
#include "graphics/imagebuilder/pathimagesource.hpp"
#include "configmanager.hpp"
#include "log.hpp"
#include "tools/utility.hpp"
#include "eventmanager.hpp"
#include <string>
#include <memory>
#include <SFGUI/Widget.hpp>
#include <SFGUI/Window.hpp>
#include <SFGUI/Container.hpp>
#include <SFGUI/Label.hpp>
#include <SFGUI/Box.hpp>
#include <SFGUI/Alignment.hpp>
#include <SFGUI/Misc.hpp>
#include <SFGUI/Button.hpp>
#include <SFGUI/Scrollbar.hpp>
#include <SFGUI/Scale.hpp>
#include <tinyxml2.h>
#include <vector>
#include <functional>

namespace BlueBear {
  namespace Graphics {

    WidgetBuilder::WidgetBuilder( EventManager& eventManager, ImageCache& imageCache ) : eventManager( eventManager ), imageCache( imageCache ) {}

    constexpr unsigned int WidgetBuilder::hash(const char* str, int h) {
      return !str[h] ? 5381 : (hash(str, h+1) * 33) ^ str[h];
    }

    /**
     * This is where it all happens. Call this method to turn your XML file into a collection of widgets.
     */
    std::vector< std::shared_ptr< sfg::Widget > > WidgetBuilder::getWidgets( const std::string& path ) {
      tinyxml2::XMLDocument document;
      document.LoadFile( path.c_str() );

      // Object is unusable if the file failed to load
      if( document.ErrorID() ) {
        Log::getInstance().error( "WidgetBuilder::getWidgets",
          "WidgetBuilder construction failed; could not parse XML file " + path
        );
        throw FailedToLoadXMLException();
      }


      std::vector< std::shared_ptr< sfg::Widget > > widgets;
      groups.clear();

      for ( tinyxml2::XMLElement* node = document.RootElement(); node != NULL; node = node->NextSiblingElement() ) {
        widgets.push_back( nodeToWidget( node ) );
      }

      // Second pass to associate groups in this widget set
      for( auto& group : groups ) {
        for( std::shared_ptr< sfg::Widget > widget : widgets ) {
          std::shared_ptr< sfg::Widget > associatedWidget = widget->GetWidgetById( group.second );
          if( associatedWidget && associatedWidget->GetName() == "RadioButton" ) {
            group.first->SetGroup( std::static_pointer_cast< sfg::RadioButton >( associatedWidget )->GetGroup() );
          }
        }
      }

      return widgets;
    }

    std::shared_ptr< sfg::Widget > WidgetBuilder::getWidgetFromXML( const std::string& xmlString ) {
      tinyxml2::XMLDocument document;
      document.Parse( xmlString.c_str() );

      if( document.ErrorID() ) {
        Log::getInstance().error( "WidgetBuilder::getWidgetFromXML",
          "WidgetBuilder construction failed; could not parse XML string " + xmlString
        );
        throw FailedToLoadXMLException();
      }

      groups.clear();

      std::shared_ptr< sfg::Widget > widget = nodeToWidget( document.RootElement() );

      for( auto& group : groups ) {
        std::shared_ptr< sfg::Widget > associatedWidget = widget->GetWidgetById( group.second );
        if( associatedWidget && associatedWidget->GetName() == "RadioButton" ) {
          group.first->SetGroup( std::static_pointer_cast< sfg::RadioButton >( associatedWidget )->GetGroup() );
        }
      }

      return widget;
    }

    std::shared_ptr< sfg::Widget > WidgetBuilder::getWidgetFromElementDirect( tinyxml2::XMLElement* element ) {
      groups.clear();

      std::shared_ptr< sfg::Widget > widget = nodeToWidget( element );

      for( auto& group : groups ) {
        std::shared_ptr< sfg::Widget > associatedWidget = widget->GetWidgetById( group.second );
        if( associatedWidget && associatedWidget->GetName() == "RadioButton" ) {
          group.first->SetGroup( std::static_pointer_cast< sfg::RadioButton >( associatedWidget )->GetGroup() );
        }
      }

      return widget;
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

        case hash( "Box" ):
          widget = newBoxWidget( element );
          packChildren( std::static_pointer_cast< sfg::Box >( widget ), element );
          break;

        case hash( "Alignment" ):
          widget = newAlignmentWidget( element );
          addChildren( std::static_pointer_cast< sfg::Container >( widget ), element );
          break;

        case hash( "Button" ):
          widget = newButtonWidget( element );
          // SFGUI has the ability to add children to a button but I don't think it makes much sense.
          /* addChildren( std::static_pointer_cast< sfg::Container >( widget ), element ); */
          break;

        case hash( "Entry" ):
          widget = newEntryWidget( element );
          break;

        case hash( "Image" ):
          widget = newImageWidget( element );
          break;

        case hash( "Frame" ):
          widget = newFrameWidget( element );
          addChildren( std::static_pointer_cast< sfg::Container >( widget ), element );
          break;

        case hash( "ProgressBar" ):
          widget = newProgressBarWidget( element );
          break;

        case hash( "Separator" ):
          widget = newSeparatorWidget( element );
          break;

        case hash( "Notebook" ):
          widget = newNotebookWidget( element );
          break;

        case hash( "ScrolledWindow" ):
          widget = newScrolledWindowWidget( element );
          addChildren( std::static_pointer_cast< sfg::ScrolledWindow >( widget ), element );
          break;

        case hash( "Viewport" ):
          widget = newViewportWidget( element );
          addChildren( std::static_pointer_cast< sfg::Container >( widget ), element );
          break;

        case hash( "Table" ):
          widget = newTableWidget( element );
          break;

        case hash( "Scrollbar" ):
          widget = newRangeWidget< sfg::Scrollbar >( element );
          break;

        case hash( "Scale" ):
          widget = newRangeWidget< sfg::Scale >( element );
          break;

        case hash( "ToggleButton" ):
          widget = newToggleButtonDerivativeWidget< sfg::ToggleButton >( element );
          break;

        case hash( "CheckButton" ):
          widget = newToggleButtonDerivativeWidget< sfg::CheckButton >( element );
          break;

        case hash( "RadioButton" ):
          widget = newRadioButtonWidget( element );
          break;

        case hash( "SpinButton" ):
          widget = newSpinButtonWidget( element );
          break;

        case hash( "ComboBox" ):
          widget = newComboBoxWidget( element );
          break;

        default:
          Log::getInstance().error( "WidgetBuilder::nodeToWidget", "Invalid CME tag specified: " + std::string( tagType ) );
          throw InvalidCMEWidgetException();
      }

      return widget;
    }

    /**
     * Add only the first widget using the function AddWithViewport
     */
    void WidgetBuilder::addChildren( std::shared_ptr< sfg::ScrolledWindow > scrolledWindow, tinyxml2::XMLElement* element ) {
      tinyxml2::XMLElement* firstChild = element->FirstChildElement();

      if( firstChild ) {
        scrolledWindow->AddWithViewport( nodeToWidget( firstChild ) );

        if( firstChild->NextSiblingElement() ) {
          Log::getInstance().warn( "WidgetBuilder::addChildren( std::shared_ptr< sfg::ScrolledWindow >, tinyxml2::XMLElement* )", "ScrolledWindow widget supports only one child; ignoring subsequent widgets." );
        }
      }
    }

    void WidgetBuilder::addChildren( std::shared_ptr< sfg::Container > widget, tinyxml2::XMLElement* element ) {
      for ( tinyxml2::XMLElement* child = element->FirstChildElement(); child != NULL; child = child->NextSiblingElement() ) {
        widget->Add( nodeToWidget( child ) );
      }
    }

    void WidgetBuilder::packChildren( std::shared_ptr< sfg::Box > widget, tinyxml2::XMLElement* element ) {
      bool packFromStart = false;
      const char* _pack = element->Attribute( "pack" );
      if( !_pack ) {
        _pack = "end";
      }
      std::string pack( _pack );

      if( pack == "start" ) {
        packFromStart = true;
      } else if ( pack == "end" ) {
        packFromStart = false;
      } else {
        Log::getInstance().warn( "WidgetBuilder::packChildren", "Invalid value for \"pack\" attribute: " + pack + ", defaulting to \"end\"" );
      }

      for ( tinyxml2::XMLElement* child = element->FirstChildElement(); child != NULL; child = child->NextSiblingElement() ) {
        bool expand = true;
        bool fill = true;

        child->QueryBoolAttribute( "expand", &expand );
        child->QueryBoolAttribute( "fill", &fill );

        if( packFromStart ) {
          widget->PackStart( nodeToWidget( child ), expand, fill );
        } else {
          widget->PackEnd( nodeToWidget( child ), expand, fill );
        }
      }
    }

    /**
     * Set basic properties on all widgets
     */
    void WidgetBuilder::setBasicProperties( std::shared_ptr< sfg::Widget > widget, tinyxml2::XMLElement* element ) {
      const char* id = element->Attribute( "id" );
      const char* clss = element->Attribute( "class" );
      const char* visible = element->Attribute( "visible" );

      if ( id ) {
        widget->SetId( id );
      }

      if ( clss ) {
        widget->SetClass( clss );
      }

      if ( visible ) {
        std::string value( visible );

        if( value == "true" ) {
          widget->Show( true );
        } else if ( value == "false" ) {
          widget->Show( false );
        } else {
          Log::getInstance().warn( "WidgetBuilder::setBasicProperties",  "Invalid value for \"visible\" attribute: " + std::string( value ) + ", defaulting to \"true\"" );
        }
      }
    }

    void WidgetBuilder::setRangeAdjustment( std::shared_ptr< sfg::Range > range, tinyxml2::XMLElement* element ) {
      std::shared_ptr< sfg::Adjustment > adjustment = range->GetAdjustment();

      float min = adjustment->GetLower();
      float max = adjustment->GetUpper();
      float minorStep = adjustment->GetMinorStep();
      float majorStep = adjustment->GetMajorStep();
      float value = adjustment->GetValue();
      float pageSize = adjustment->GetPageSize();

      element->QueryFloatAttribute( "min", &min );
      element->QueryFloatAttribute( "max", &max );
      element->QueryFloatAttribute( "minor_step", &minorStep );
      element->QueryFloatAttribute( "major_step", &majorStep );
      element->QueryFloatAttribute( "page_size", &pageSize );
      element->QueryFloatText( &value );

      adjustment->SetLower( min );
      adjustment->SetUpper( max );
      adjustment->SetMinorStep( minorStep );
      adjustment->SetMajorStep( majorStep );
      adjustment->SetPageSize( pageSize );
      adjustment->SetValue( value );
    }

    void WidgetBuilder::correctXBoundary( float* input ) {
      static int VIEWPORT_X = ConfigManager::getInstance().getIntValue( "viewport_x" );

      if( *input < 0.0f ) {
        *input = VIEWPORT_X + *input;
      }
    }

    void WidgetBuilder::correctYBoundary( float* input ) {
      static int VIEWPORT_Y = ConfigManager::getInstance().getIntValue( "viewport_y" );

      if( *input < 0.0f ) {
        *input = VIEWPORT_Y + *input;
      }
    }

    /**
     * Attach events that should be on every widget
     */
    void WidgetBuilder::setDefaultEvents( std::shared_ptr< sfg::Widget > widget, tinyxml2::XMLElement* element ) {
      widget->GetSignal( sfg::Widget::OnMouseLeftRelease ).Connect( [ &eventManager = eventManager ]() {
        eventManager.SFGUI_EAT_EVENT.trigger( SFGUIEatEvent::Event::EAT_MOUSE_EVENT );
      } );

      widget->GetSignal( sfg::Widget::OnMouseRightRelease ).Connect( [ &eventManager = eventManager ]() {
        eventManager.SFGUI_EAT_EVENT.trigger( SFGUIEatEvent::Event::EAT_MOUSE_EVENT );
      } );
    }

    void WidgetBuilder::setAlignment( std::shared_ptr< sfg::Misc > widget, tinyxml2::XMLElement* element ) {
      sf::Vector2f alignment = widget->GetAlignment();

      element->QueryFloatAttribute( "alignment_x", &alignment.x );
      element->QueryFloatAttribute( "alignment_y", &alignment.y );

      widget->SetAlignment( alignment );
    }

    void WidgetBuilder::setAllocationAndRequisition( std::shared_ptr< sfg::Widget > widget, tinyxml2::XMLElement* element ) {
      sf::FloatRect allocation = widget->GetAllocation();

      element->QueryFloatAttribute( "top", &allocation.top );
      correctYBoundary( &allocation.top );
      element->QueryFloatAttribute( "left", &allocation.left );
      correctXBoundary( &allocation.left );
      element->QueryFloatAttribute( "width", &allocation.width );
      element->QueryFloatAttribute( "height", &allocation.height );

      widget->SetAllocation( allocation );

      sf::Vector2f requisition = widget->GetRequisition();

      element->QueryFloatAttribute( "min-width", &requisition.x );
      element->QueryFloatAttribute( "min-height", &requisition.y );

      widget->SetRequisition( requisition );
    }

    void WidgetBuilder::setEatEntryEvent( std::shared_ptr< sfg::Entry > entry ) {
      // Use eventManager to trigger a disable key events on eventManager.SFGUI_SIGNAL_EVENT when focusing in,
      // and an enable key events when focusing out.
      entry->GetSignal( sfg::Widget::OnKeyPress ).Connect( [ &eventManager = eventManager ]() {
        eventManager.SFGUI_EAT_EVENT.trigger( SFGUIEatEvent::Event::EAT_KEYBOARD_EVENT );
      } );
    }

    std::shared_ptr< sfg::Window > WidgetBuilder::newWindowWidget( tinyxml2::XMLElement* element ) {
      std::shared_ptr< sfg::Window > window = sfg::Window::Create();

      setBasicProperties( window, element );
      setAllocationAndRequisition( window, element );
      setDefaultEvents( window, element );

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
      label = sfg::Label::Create( labelValue ? labelValue : "" );

      setBasicProperties( label, element );
      setAllocationAndRequisition( label, element );
      setAlignment( label, element );

      setDefaultEvents( label, element );

      return label;
    }

    std::shared_ptr< sfg::Box > WidgetBuilder::newBoxWidget( tinyxml2::XMLElement* element ) {
      std::shared_ptr< sfg::Box > box;

      const char* orientation = element->Attribute( "orientation" );
      if( !orientation ) {
        orientation = "horizontal";
      }

      sfg::Box::Orientation orientationFlag;

      switch( hash( orientation ) ) {
        case hash( "vertical" ):
          orientationFlag = sfg::Box::Orientation::VERTICAL;
          break;
        default:
          Log::getInstance().warn( "WidgetBuilder::newBoxWidget", "Invalid value for \"orientation\" attribute: " + std::string( orientation ) + ", defaulting to \"horizontal\"" );
        case hash( "horizontal" ):
          orientationFlag = sfg::Box::Orientation::HORIZONTAL;
          break;
      }

      float spacing = 0.0f;
      element->QueryFloatAttribute( "spacing", &spacing );

      box = sfg::Box::Create( orientationFlag, spacing );
      setBasicProperties( box, element );
      setDefaultEvents( box, element );

      return box;
    }

    std::shared_ptr< sfg::Alignment > WidgetBuilder::newAlignmentWidget( tinyxml2::XMLElement* element ) {
      std::shared_ptr< sfg::Alignment > alignment = sfg::Alignment::Create();

      setBasicProperties( alignment, element );
      setAllocationAndRequisition( alignment, element );
      setAlignment( alignment, element );
      setDefaultEvents( alignment, element );

      sf::Vector2f scale = alignment->GetScale();
      element->QueryFloatAttribute( "scale_x", &scale.x );
      element->QueryFloatAttribute( "scale_y", &scale.y );

      alignment->SetScale( scale );

      return alignment;
    }

    /**
     * TODO: Support for sfg::Image, and the ability to add one to a Button
     */
    std::shared_ptr< sfg::Button > WidgetBuilder::newButtonWidget( tinyxml2::XMLElement* element ) {
      std::shared_ptr< sfg::Button > button = sfg::Button::Create( Tools::Utility::sanitizeCString( element->GetText() ) );

      setBasicProperties( button, element );
      setAllocationAndRequisition( button, element );
      setDefaultEvents( button, element );

      return button;
    }

    std::shared_ptr< sfg::Entry > WidgetBuilder::newEntryWidget( tinyxml2::XMLElement* element ) {
      std::shared_ptr< sfg::Entry > entry = sfg::Entry::Create( Tools::Utility::sanitizeCString( element->GetText() ) );

      setBasicProperties( entry, element );
      setAllocationAndRequisition( entry, element );
      setDefaultEvents( entry, element );

      setEatEntryEvent( entry );

      return entry;
    }

    std::shared_ptr< sfg::Image > WidgetBuilder::newImageWidget( tinyxml2::XMLElement* element ) {
      std::shared_ptr< sfg::Image > image = sfg::Image::Create();

      setBasicProperties( image, element );
      setAllocationAndRequisition( image, element );
      setAlignment( image, element );
      setDefaultEvents( image, element );

      // This should be somewhat fault-tolerant since images go missing all the damn time
      std::string path = element->GetText();

      try {
        PathImageSource piss = PathImageSource( path );
        image->SetImage( *imageCache.getImage( piss ) );
      } catch( std::exception& e ) {
        Log::getInstance().warn( "WidgetBuilder::newImageWidget", "Could not load image for image widget: " + std::string( e.what() ) );
      }

      return image;
    }

    std::shared_ptr< sfg::Frame > WidgetBuilder::newFrameWidget( tinyxml2::XMLElement* element ) {
      std::shared_ptr< sfg::Frame > frame = sfg::Frame::Create( element->Attribute( "title" ) );

      setBasicProperties( frame, element );
      setAllocationAndRequisition( frame, element );
      setAlignment( frame, element );
      setDefaultEvents( frame, element );

      return frame;
    }

    std::shared_ptr< sfg::ProgressBar > WidgetBuilder::newProgressBarWidget( tinyxml2::XMLElement* element ) {
      std::shared_ptr< sfg::ProgressBar > progressBar = sfg::ProgressBar::Create( getOrientation< sfg::ProgressBar::Orientation >( element->Attribute( "orientation" ) ) );

      setBasicProperties( progressBar, element );
      setAllocationAndRequisition( progressBar, element );
      setDefaultEvents( progressBar, element );

      float fraction = 0.0f;
      element->QueryFloatAttribute( "value", &fraction );

      progressBar->SetFraction( fraction );

      return progressBar;
    }

    std::shared_ptr< sfg::Separator > WidgetBuilder::newSeparatorWidget( tinyxml2::XMLElement* element ) {
      std::shared_ptr< sfg::Separator > separator = sfg::Separator::Create( getOrientation< sfg::Separator::Orientation >( element->Attribute( "orientation" ) ) );

      setBasicProperties( separator, element );
      setAllocationAndRequisition( separator, element );
      setDefaultEvents( separator, element );

      return separator;
    }

    std::shared_ptr< sfg::Notebook > WidgetBuilder::newNotebookWidget( tinyxml2::XMLElement* element ) {
      std::shared_ptr< sfg::Notebook > notebook = sfg::Notebook::Create();

      setBasicProperties( notebook, element );
      setAllocationAndRequisition( notebook, element );
      setDefaultEvents( notebook, element );

      notebook->SetTabPosition( getPosition< sfg::Notebook::TabPosition >( element->Attribute( "tab_position" ) ) );

      bool scrollable = notebook->GetScrollable();
      element->QueryBoolAttribute( "scrollable", &scrollable );
      notebook->SetScrollable( scrollable );

      addNotebookTabs( notebook, element );

      unsigned int currentPage = 0;
      element->QueryUnsignedAttribute( "selected", &currentPage );
      if( currentPage < notebook->GetPageCount() ) {
        notebook->SetCurrentPage( currentPage );
      } else {
        Log::getInstance().warn( "WidgetBuilder::newNotebookWidget", "Invalid page index for notebook specified." );
      }

      return notebook;
    }

    std::shared_ptr< sfg::ScrolledWindow > WidgetBuilder::newScrolledWindowWidget( tinyxml2::XMLElement* element ) {
      // Assert that element has a min-width and min-height, as this widget requires a requisition.
      if( !element->Attribute( "min-width" ) || !element->Attribute( "min-height" ) ) {
        Log::getInstance().error( "WidgetBuilder::newScrolledWindowWidget", "ScrolledWindow widget requires min-width and min-height" );
        throw FailedToLoadXMLException();
      }

      std::shared_ptr< sfg::ScrolledWindow > scrolledWindow = sfg::ScrolledWindow::Create();
      setBasicProperties( scrolledWindow, element );
      setAllocationAndRequisition( scrolledWindow, element );
      setDefaultEvents( scrolledWindow, element );

      // Set a default scrollbar policy to "auto"
      scrolledWindow->SetScrollbarPolicy( ScrollbarPolicy( element->Attribute( "scrollbar_x" ), element->Attribute( "scrollbar_y" ) ).get() );
      scrolledWindow->SetPlacement( Placement( element->Attribute( "placement" ) ).get() );

      setAdjustments< sfg::ScrolledWindow >( scrolledWindow, element );

      return scrolledWindow;
    }

    std::shared_ptr< sfg::Viewport > WidgetBuilder::newViewportWidget( tinyxml2::XMLElement* element ) {
      // Assert that element has a min-width and min-height, as this widget requires a requisition.
      if( !element->Attribute( "min-width" ) || !element->Attribute( "min-height" ) ) {
        Log::getInstance().error( "WidgetBuilder::newViewportWidget", "Viewport widget requires min-width and min-height" );
        throw FailedToLoadXMLException();
      }

      std::shared_ptr< sfg::Viewport > viewport = sfg::Viewport::Create();

      setBasicProperties( viewport, element );
      setAllocationAndRequisition( viewport, element );
      setDefaultEvents( viewport, element );

      setAdjustments< sfg::Viewport >( viewport, element );

      return viewport;
    }

    std::shared_ptr< sfg::Table > WidgetBuilder::newTableWidget( tinyxml2::XMLElement* element ) {
      std::shared_ptr< sfg::Table > table = sfg::Table::Create();

      setBasicProperties( table, element );
      setAllocationAndRequisition( table, element );
      setDefaultEvents( table, element );

      // Add table rows
      addTableRows( table, element );

      return table;
    }

    std::shared_ptr< sfg::RadioButton > WidgetBuilder::newRadioButtonWidget( tinyxml2::XMLElement* element ) {
      std::shared_ptr< sfg::RadioButton > radioButton = newToggleButtonDerivativeWidget< sfg::RadioButton >( element );

      // If there is the presence of a "group" attribute, associate this radio button widget to the same group of the ID specified.
      // This will be picked up after the host operation's first nodeToWidget call is complete.
      if( const char* group = element->Attribute( "group" ) ) {
        groups.emplace_back( radioButton, group );
      }

      return radioButton;
    }

    std::shared_ptr< sfg::SpinButton > WidgetBuilder::newSpinButtonWidget( tinyxml2::XMLElement* element ) {
      float minimum = 0.0f;
      float maximum = 100.0f;
      float step = 1.0f;

      element->QueryFloatAttribute( "min", &minimum );
      element->QueryFloatAttribute( "max", &maximum );
      element->QueryFloatAttribute( "step", &step );

      std::shared_ptr< sfg::SpinButton > spinButton = sfg::SpinButton::Create( minimum, maximum, step );

      setBasicProperties( spinButton, element );
      setAllocationAndRequisition( spinButton, element );
      setDefaultEvents( spinButton, element );

      unsigned int precision = 0;
      element->QueryUnsignedAttribute( "precision", &precision );
      spinButton->SetDigits( precision );

      float value = spinButton->GetValue();
      element->QueryFloatText( &value );
      spinButton->SetValue( value );

      setEatEntryEvent( spinButton );

      return spinButton;
    }

    std::shared_ptr< sfg::ComboBox > WidgetBuilder::newComboBoxWidget( tinyxml2::XMLElement* element ) {
      std::shared_ptr< sfg::ComboBox > comboBox = sfg::ComboBox::Create();

      setBasicProperties( comboBox, element );
      setAllocationAndRequisition( comboBox, element );
      setDefaultEvents( comboBox, element );

      for( tinyxml2::XMLElement* item = element->FirstChildElement(); item != NULL; item = item->NextSiblingElement() ) {
        std::string nodeName( item->Name() );

        if( nodeName == "item" ) {
          if( const char* text = item->GetText() ) {
            comboBox->AppendItem( text );
          } else {
            Log::getInstance().warn( "WidgetBuilder::newComboBoxWidget", "Invalid <ComboBox> child element; <item> has no valid text." );
          }
        } else {
          Log::getInstance().warn( "WidgetBuilder::newComboBoxWidget", "Invalid <ComboBox> child element; only <item> pseudo-elements are valid direct children of a <ComboBox>." );
        }
      }

      int currentPage = -1;
      element->QueryIntAttribute( "selected", &currentPage );
      if( currentPage >= 0 ) {
        comboBox->SelectItem( currentPage );
      }

      return comboBox;
    }

    void WidgetBuilder::addTableRows( std::shared_ptr< sfg::Table > table, tinyxml2::XMLElement* element ) {
      std::map< unsigned int, float > requestedRowSpacings;
      std::map< unsigned int, float > requestedColumnSpacings;

      unsigned int currentRow = 0;

      for( tinyxml2::XMLElement* row = element->FirstChildElement(); row != NULL; row = row->NextSiblingElement() ) {
        std::string nodeName( row->Name() );

        if( nodeName == "row" ) {
          unsigned int currentColumn = 0;

          for( tinyxml2::XMLElement* cell = row->FirstChildElement(); cell != NULL; cell = cell->NextSiblingElement() ) {
            unsigned int colspan = 1;
            cell->QueryUnsignedAttribute( "colspan", &colspan );
            unsigned int rowspan = 1;
            cell->QueryUnsignedAttribute( "rowspan", &rowspan );

            float paddingX = 0.0f;
            cell->QueryFloatAttribute( "padding_x", &paddingX );
            float paddingY = 0.0f;
            cell->QueryFloatAttribute( "padding_y", &paddingY );

            bool expandX = true, expandY = true;
            bool fillX = true, fillY = true;

            cell->QueryBoolAttribute( "expand_x", &expandX );
            cell->QueryBoolAttribute( "expand_y", &expandY );
            cell->QueryBoolAttribute( "fill_x", &fillX );
            cell->QueryBoolAttribute( "fill_y", &fillY );

            int packX = 0, packY = 0;

            if( expandX ) { packX |= sfg::Table::EXPAND; }
            if( fillX ) { packX |= sfg::Table::FILL; }

            if( expandY ) { packY |= sfg::Table::EXPAND; }
            if( fillY ) { packY |= sfg::Table::FILL; }

            table->Attach( nodeToWidget( cell ), sf::Rect< sf::Uint32 >( currentColumn, currentRow, colspan, rowspan ), packX, packY, sf::Vector2f( paddingX, paddingY ) );

            float spacing = 0.0f;
            if( cell->QueryFloatAttribute( "spacing", &spacing ) == tinyxml2::XML_SUCCESS ) {
              requestedColumnSpacings[ currentColumn ] = spacing;
            }

            currentColumn++;
          }

          float spacing = 0.0f;
          if( row->QueryFloatAttribute( "spacing", &spacing ) == tinyxml2::XML_SUCCESS ) {
            requestedRowSpacings[ currentRow ] = spacing;
          }

          currentRow++;
        } else {
          Log::getInstance().warn( "WidgetBuilder::addTableRows", "Invalid <Table> child element; only <row> pseudo-elements are valid direct children of a <Table>." );
        }
      }

      float rowSpacing = 0.0f;
      float columnSpacing = 0.0f;
      element->QueryFloatAttribute( "row_spacing", &rowSpacing );
      element->QueryFloatAttribute( "column_spacing", &columnSpacing );

      table->SetRowSpacings( rowSpacing );
      table->SetColumnSpacings( columnSpacing );

      for( auto& it : requestedRowSpacings ) {
        table->SetRowSpacing( it.first, it.second );
      }
      for( auto& it : requestedColumnSpacings ) {
        table->SetColumnSpacing( it.first, it.second );
      }
    }

    void WidgetBuilder::addNotebookTabs( std::shared_ptr< sfg::Notebook > notebook, tinyxml2::XMLElement* element ) {
      for ( tinyxml2::XMLElement* child = element->FirstChildElement(); child != NULL; child = child->NextSiblingElement() ) {
        // should be "page" pseudo-element
        std::string name( child->Name() );

        if( name == "page" ) {
          std::shared_ptr< sfg::Widget > tab = nullptr;
          std::shared_ptr< sfg::Widget > content = nullptr;

          // generate BOTH tab and content
          for( tinyxml2::XMLElement* pageChild = child->FirstChildElement(); pageChild != NULL; pageChild = pageChild->NextSiblingElement() ) {
            const char* name = pageChild->Name();

            switch( hash( name ) ) {
              case hash( "tab" ):
                // <tab> pseudo-element
                if( tab ) {
                  Log::getInstance().warn( "WidgetBuilder::addNotebookTabs", "Disregarding duplicate <tab> pseudo-element within <page>" );
                } else {
                  tab = nodeToWidget( pageChild->FirstChildElement() );
                }
                break;
              case hash( "content" ):
                // <content> pseudo-element
                if( content ) {
                  Log::getInstance().warn( "WidgetBuilder::addNotebookTabs", "Disregarding duplicate <content> pseudo-element within <page>" );
                } else {
                  content = nodeToWidget( pageChild->FirstChildElement() );
                }
                break;
              default:
                Log::getInstance().warn( "WidgetBuilder::addNotebookTabs", "Invalid pseudo-element within page pseudo-element: " + std::string( name ) );
            }
          }

          // If neither tab nor content were generated, this page is unusable
          if( !tab || !content ) {
            Log::getInstance().warn( "WidgetBuilder::addNotebookTabs", "Incomplete <page> pseudo-element requires both a <tab> and <content> pseudo-element." );
          } else {
            notebook->AppendPage( content, tab );
          }
        } else {
          Log::getInstance().warn( "WidgetBuilder::addNotebookTabs", "Invalid pseudo-element within Notebook element: " + name );
        }
      }
    }

  }
}
