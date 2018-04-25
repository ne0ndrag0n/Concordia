#include "graphics/userinterface/widgets/input.hpp"
#include "graphics/userinterface/style/style.hpp"
#include "device/display/adapter/component/guicomponent.hpp"
#include "device/input/input.hpp"
#include "configmanager.hpp"
#include <glm/glm.hpp>
#include <map>

#include "log.hpp"

namespace BlueBear::Graphics::UserInterface::Widgets {

  Input::Input( const std::string& id, const std::vector< std::string >& classes, const std::string& hintText, const std::string& contents ) :
    Element::Element( "Input", id, classes ), hintText( hintText ), contents( contents ) {
      eventBundle.registerInputEvent( "focus", std::bind( &Input::onFocus, this, std::placeholders::_1 ) );
      eventBundle.registerInputEvent( "blur", std::bind( &Input::onBlur, this, std::placeholders::_1 ) );
    }

  std::shared_ptr< Input > Input::create( const std::string& id, const std::vector< std::string >& classes, const std::string& hintText, const std::string& contents ) {
    std::shared_ptr< Input > input( new Input( id, classes, hintText, contents ) );

    return input;
  }

  void Input::onFocus( Device::Input::Metadata event ) {
    double fps = ConfigManager::getInstance().getIntValue( "fps_overview" );
    double duration = fps;

    focused = true;
    localStyle.attachAnimation( std::make_unique< Style::Style::Animation >(
      &localStyle,
      std::map< double, Style::Style::Animation::Keyframe >{
        {
          0.0,
          {
            PropertyList( { { "cursor-color", localStyle.get< glm::uvec4 >( "fade-out-color" ) } } ),
            true
          }
        },
        {
          duration / 2,
          {
            PropertyList( { { "cursor-color", localStyle.get< glm::uvec4 >( "fade-in-color" ) } } ),
            true
          }
        },
        {
          duration,
          {
            PropertyList( { { "cursor-color", localStyle.get< glm::uvec4 >( "fade-out-color" ) } } ),
            true
          }
        }
      },
      fps,
      duration,
      false,
      false
    ) );
  }

  void Input::onBlur( Device::Input::Metadata event ) {
    focused = false;
    localStyle.attachAnimation( nullptr );

    reflow();
  }

  int Input::getSubstringWidth( const std::string& letter ) {
    glm::vec4 size = manager->getVectorRenderer().getTextSizeParams( localStyle.get< std::string >( "font" ), letter, localStyle.get< double >( "font-size" ) );
    return size[ 2 ];
  }

  void Input::render( Graphics::Vector::Renderer& renderer ) {

    double fontSize = localStyle.get< double >( "font-size" );

    // Line
    renderer.drawRect(
      { 4, allocation[ 3 ] - 12, allocation[ 2 ] - 4, allocation[ 3 ] - 8 },
      localStyle.get< glm::uvec4 >( "color" )
    );

    if( focused ) {
      // Text
      renderer.drawText(
        localStyle.get< std::string >( "font" ),
        contents,
        { 6, 6 + ( fontSize / 2 ) },
        localStyle.get< glm::uvec4 >( "font-color" ),
        fontSize
      );

      // Cursor
      glm::uvec2 cursorOrigin{ 5 + getSubstringWidth( contents.substr( 0, cursorPosition ) ), 8 };
      renderer.drawRect(
        { cursorOrigin.x, cursorOrigin.y, cursorOrigin.x + 2, allocation[ 3 ] - 16 },
        localStyle.get< glm::uvec4 >( "cursor-color" )
      );
    } else {
      // Hint (or text)
      renderer.drawText(
        localStyle.get< std::string >( "font" ),
        contents.size() ? contents : hintText,
        { 6, 6 + ( fontSize / 2 ) },
        contents.size() ? localStyle.get< glm::uvec4 >( "font-color" ) : localStyle.get< glm::uvec4 >( "font-hint-color" ),
        fontSize
      );
    }
  }

  void Input::calculate() {
    double fontSize = localStyle.get< double >( "font-size" );
    glm::vec4 size = manager->getVectorRenderer().getTextSizeParams( localStyle.get< std::string >( "font" ), hintText, fontSize );
    textSpan = size[ 2 ];

    requisition = glm::uvec2{ textSpan + 16, fontSize + 20 };
  }

  void Input::reflow() {
    if( auto parent = getParent() ) {
      parent->reflow();
    } else {
      Element::reflow();
    }
  }

}
