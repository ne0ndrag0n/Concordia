#include "graphics/userinterface/widgets/tablayout.hpp"
#include "device/input/input.hpp"
#include "device/display/adapter/component/guicomponent.hpp"
#include "graphics/vector/renderer.hpp"

namespace BlueBear::Graphics::UserInterface::Widgets {

  TabLayout::TabLayout( const std::string& id, const std::vector< std::string >& classes ) :
    Element::Element( "TabLayout", id, classes ) {
      eventBundle.registerInputEvent( "mouse-up", std::bind( &TabLayout::onMouseUp, this, std::placeholders::_1 ) );
    }

  void TabLayout::onMouseUp( Device::Input::Metadata event ) {
    if( !children.size() ) {
      return;
    }

    auto relative = toRelative( event.mouseLocation );
    int y = ( localStyle.get< int >( "padding" ) * 2 ) + localStyle.get< double >( "font-size" ) + 5;
    int boxWidth = allocation[ 2 ] / children.size();

    int index = 0;
    for( int xPos = 0; xPos <= allocation[ 2 ]; xPos += boxWidth ) {
      if( relative.x >= xPos && relative.x <= xPos + boxWidth && relative.y <= y ) {
        return selectElement( index );
      }

      index++;
    }
  }

  void TabLayout::onMouseDown( Device::Input::Metadata event ) {}

  void TabLayout::calculate() {
    int padding = localStyle.get< int >( "padding" );
    double fontSize = localStyle.get< double >( "font-size" );
    glm::uvec2 result{ 0, 5 + ( padding * 2 ) + fontSize };

    for( std::shared_ptr< Element > child : children ) {
      glm::vec4 textDimensions = manager->getVectorRenderer().getTextSizeParams(
        localStyle.get< std::string >( "font" ),
        child->getPropertyList().get< std::string >( "tab-title" ),
        fontSize
      );

      textSpans.push_back( textDimensions );
      result.x += textDimensions[ 2 ];
    }

    requisition = result;
  }

  void TabLayout::positionAndSizeChildren() {
    for( int i = 0; i != children.size(); i++ ) {
      std::shared_ptr< Element > child = children[ i ];

      if( i != selectedIndex ) {
        child->setVisible( false );
      } else {
        child->setVisible( true );
      }

      int y = ( localStyle.get< int >( "padding" ) * 2 ) + localStyle.get< double >( "font-size" ) + 5;
      child->setAllocation( { 0, y, allocation[ 2 ], allocation[ 3 ] - y }, false );
    }
  }

  void TabLayout::render( Graphics::Vector::Renderer& renderer ) {
    int padding = localStyle.get< int >( "padding" );
    double fontSize = localStyle.get< double >( "font-size" );
    int bottomY = ( padding * 2 ) + fontSize + 5;

    // Backdrop
    renderer.drawRect(
      { 0, 0, allocation[ 2 ], bottomY },
      localStyle.get< glm::uvec4 >( "color" )
    );

    if( !children.size() ) {
      return;
    }

    int boxWidth = allocation[ 2 ] / children.size();
    int xPos = 0;
    int index = 0;
    for( std::shared_ptr< Element > child : children ) {
      // Text
      renderer.drawText(
        localStyle.get< std::string >( "font" ),
        child->getPropertyList().get< std::string >( "tab-title" ),
        { ( xPos + ( boxWidth / 2 ) ) - ( textSpans[ index ][ 2 ] / 2 ), padding + ( fontSize / 2 ) + 3 },
        localStyle.get< glm::uvec4 >( "font-color" ),
        fontSize
      );

      // Accent
      renderer.drawRect(
        { xPos + padding, bottomY - 3, xPos + boxWidth - padding, bottomY },
        index == selectedIndex ? localStyle.get< glm::uvec4 >( "tab-active-accent-color" ) : localStyle.get< glm::uvec4 >( "tab-inactive-accent-color" )
      );

      xPos += boxWidth;
      index++;
    }
  }

  void TabLayout::selectElement( unsigned int index ) {
    selectedIndex = index;
    reflow();
  }

  std::shared_ptr< TabLayout > TabLayout::create( const std::string& id, const std::vector< std::string >& classes ) {
    std::shared_ptr< TabLayout > tabLayout( new TabLayout( id, classes ) );

    return tabLayout;
  }

}
