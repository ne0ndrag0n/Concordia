#include "graphics/userinterface/widgets/button.hpp"
#include "graphics/userinterface/style/style.hpp"
#include "device/display/adapter/component/guicomponent.hpp"
#include "graphics/vector/renderer.hpp"
#include "device/input/input.hpp"
#include "configmanager.hpp"
#include <functional>

namespace BlueBear::Graphics::UserInterface::Widgets {

  Button::Button( const std::string& id, const std::vector< std::string >& classes, const std::string& innerText ) :
    Element::Element( "Button", id, classes ), label( innerText ) {
      eventBundle.registerInputEvent( "mouse-in", std::bind( &Button::onMouseIn, this, std::placeholders::_1 ) );
      eventBundle.registerInputEvent( "mouse-out", std::bind( &Button::onMouseOut, this, std::placeholders::_1 ) );

      eventBundle.registerInputEvent( "mouse-down", std::bind( &Button::onMouseDown, this, std::placeholders::_1 ) );
      eventBundle.registerInputEvent( "mouse-up", std::bind( &Button::onMouseUp, this, std::placeholders::_1 ) );
    }

  std::shared_ptr< Button > Button::create( const std::string& id, const std::vector< std::string >& classes, const std::string& innerText ) {
    std::shared_ptr< Button > button( new Button( id, classes, innerText ) );

    return button;
  }

  void Button::onMouseIn( Device::Input::Metadata event ) {
    double fps = ConfigManager::getInstance().getIntValue( "fps_overview" );

    // Refresh and remove
    localStyle.attachAnimation( nullptr );

    // Attach animation
    localStyle.attachAnimation( std::make_unique< Style::Style::Animation >(
      &localStyle,
      std::map< double, Style::Style::Animation::Keyframe >{
        {
          fps,
          {
            PropertyList( { { "background-color", localStyle.get< glm::uvec4 >( "fade-in-color" ) } } ),
            true
          }
        }
      },
      fps * 3.0,
      fps,
      false,
      true
    ) );
  }

  void Button::onMouseOut( Device::Input::Metadata event ) {
    double fps = ConfigManager::getInstance().getIntValue( "fps_overview" );

    // Refresh and remove
    localStyle.attachAnimation( nullptr );

    // Attach animation
    localStyle.attachAnimation( std::make_unique< Style::Style::Animation >(
      &localStyle,
      std::map< double, Style::Style::Animation::Keyframe >{
        {
          0.0,
          {
            PropertyList( { { "background-color", localStyle.get< glm::uvec4 >( "fade-in-color" ) } } ),
            true
          }
        },
        {
          fps,
          {
            PropertyList( { { "background-color", localStyle.get< glm::uvec4 >( "background-color" ) } } ),
            true
          }
        }
      },
      fps * 3.0,
      fps,
      true,
      false
    ) );
  }

  void Button::onMouseDown( Device::Input::Metadata event ) {
    double fps = ConfigManager::getInstance().getIntValue( "fps_overview" );

    // Refresh and remove
    localStyle.attachAnimation( nullptr );

    // Attach animation
    localStyle.attachAnimation( std::make_unique< Style::Style::Animation >(
      &localStyle,
      std::map< double, Style::Style::Animation::Keyframe >{
        {
          0.0,
          {
            PropertyList( { { "background-color", localStyle.get< glm::uvec4 >( "fade-in-color" ) } } ),
            true
          }
        },
        {
          fps,
          {
            PropertyList( { { "background-color", localStyle.get< glm::uvec4 >( "fade-out-color" ) } } ),
            true
          }
        }
      },
      fps * 3.0,
      fps,
      false,
      true
    ) );
  }

  void Button::onMouseUp( Device::Input::Metadata event ) {
    double fps = ConfigManager::getInstance().getIntValue( "fps_overview" );

    // Refresh and remove
    localStyle.attachAnimation( nullptr );

    // Attach animation
    localStyle.attachAnimation( std::make_unique< Style::Style::Animation >(
      &localStyle,
      std::map< double, Style::Style::Animation::Keyframe >{
        {
          0.0,
          {
            PropertyList( { { "background-color", localStyle.get< glm::uvec4 >( "fade-out-color" ) } } ),
            true
          }
        },
        {
          fps,
          {
            PropertyList( { { "background-color", localStyle.get< glm::uvec4 >( "fade-in-color" ) } } ),
            true
          }
        }
      },
      fps * 3.0,
      fps,
      false,
      true
    ) );
  }

  void Button::render( Graphics::Vector::Renderer& renderer ) {

    glm::ivec2 origin = { 5, 5 };
    glm::ivec2 dimensions = { allocation[ 2 ] - 5, allocation[ 3 ] - 5 };

    // Drop shadow
    // Left segment
    renderer.drawLinearGradient(
      { 0, origin.y, origin.x, dimensions.y },
      { origin.x, ( origin.y + ( ( dimensions.y - origin.y ) / 2 ) ), 0, ( origin.y + ( ( dimensions.y - origin.y ) / 2 ) ) },
      { 0, 0, 0, 128 },
      { 0, 0, 0, 0 }
    );
    // Top left corner
    renderer.drawScissored( { 0, 0, origin.x, origin.y }, [ & ]() {
      renderer.drawRadialGradient( origin, 0.05f, 4.95f, { 0, 0, 0, 128 }, { 0, 0, 0, 0 } );
    } );
    // Top segment
    renderer.drawLinearGradient(
      { 5, 0, dimensions.x, 5 },
      { ( origin.x + ( ( dimensions.x - origin.x ) / 2 ) ), 5, ( origin.x + ( ( dimensions.x - origin.x ) / 2 ) ), 0 },
      { 0, 0, 0, 128 },
      { 0, 0, 0, 0 }
    );
    // Top right corner
    renderer.drawScissored( { allocation[ 2 ] - origin.x, 0, allocation[ 2 ], origin.y }, [ & ]() {
      renderer.drawRadialGradient( { allocation[ 2 ] - origin.x, origin.y }, 0.05f, 4.95f, { 0, 0, 0, 128 }, { 0, 0, 0, 0 } );
    } );
    // Right segment
    renderer.drawLinearGradient(
      { allocation[ 2 ] - origin.x, origin.y, allocation[ 2 ], dimensions.y },
      { allocation[ 2 ] - origin.x, ( origin.y + ( ( dimensions.y - origin.y ) / 2 ) ), allocation[ 2 ], ( origin.y + ( ( dimensions.y - origin.y ) / 2 ) ) },
      { 0, 0, 0, 128 },
      { 0, 0, 0, 0 }
    );
    // Bottom segment
    renderer.drawLinearGradient(
      { origin.x, dimensions.y, allocation[ 2 ] - origin.x, allocation[ 3 ] },
      { ( ( dimensions.x - origin.x ) / 2 ), dimensions.y, ( ( dimensions.x - origin.x ) / 2 ), allocation[ 3 ] },
      { 0, 0, 0, 128 },
      { 0, 0, 0, 0 }
    );
    // Bottom left corner
    renderer.drawScissored( { 0, dimensions.y, origin.x, allocation[ 3 ] }, [ & ]() {
      renderer.drawRadialGradient( { origin.x, dimensions.y }, 0.05f, 4.95f, { 0, 0, 0, 128 }, { 0, 0, 0, 0 } );
    } );
    // Bottom right corner
    renderer.drawScissored( { dimensions.x, dimensions.y, allocation[ 2 ], allocation[ 3 ] }, [ & ]() {
      renderer.drawRadialGradient( { dimensions.x, dimensions.y }, 0.05f, 4.95f, { 0, 0, 0, 128 }, { 0, 0, 0, 0 } );
    } );

    // Background color
    renderer.drawRect(
      glm::uvec4{ origin.x, origin.y, dimensions.x, dimensions.y },
      localStyle.get< glm::uvec4 >( "background-color" )
    );

    // Text
    double fontSize = localStyle.get< double >( "font-size" );
    renderer.drawText(
      localStyle.get< std::string >( "font" ),
      label,
      glm::uvec2{ ( allocation[ 2 ] / 2 ) - ( textSpan / 2 ), ( ( dimensions.y - origin.y ) / 2 ) + ( fontSize / 2 ) - 3 },
      localStyle.get< glm::uvec4 >( "color" ),
      fontSize
    );
  }

  void Button::calculate() {
    int padding = localStyle.get< int >( "padding" );
    double fontSize = localStyle.get< double >( "font-size" );
    glm::vec4 size = manager->getVectorRenderer().getTextSizeParams( localStyle.get< std::string >( "font" ), label, fontSize );
    textSpan = size[ 2 ];

    requisition = glm::uvec2{
      ( padding * 2 ) + textSpan + 10,
      ( padding * 2 ) + fontSize + 10
    };
  }

  void Button::reflow() {
    // Do not ask parent to reflow if width/height were not changed
    std::unordered_set< std::string > changedInFrame = localStyle.getChangedAttributes();
    if( changedInFrame.count( "width" ) || changedInFrame.count( "height" ) ) {
      if( auto parent = getParent() ) {
        parent->reflow();
      } else {
        Element::reflow();
      }
    } else {
      Element::reflow();
    }
  }

}
