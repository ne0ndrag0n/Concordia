#include "graphics/userinterface/widgets/pane.hpp"
#include "device/display/adapter/component/guicomponent.hpp"
#include "graphics/vector/renderer.hpp"
#include <glm/glm.hpp>

namespace BlueBear::Graphics::UserInterface::Widgets {

  Pane::Pane( const std::string& id, const std::vector< std::string >& classes ) : Element::Element( "Pane", id, classes ) {}

  std::shared_ptr< Pane > Pane::create( const std::string& id, const std::vector< std::string >& classes ) {
    std::shared_ptr< Pane > pane( new Pane( id, classes ) );

    return pane;
  }

  void Pane::positionAndSizeChildren() {
    if( children.size() ) {
      // Only do the first
      std::shared_ptr< Element > onlyChild = children[ 0 ];

      glm::ivec4 offset;
      bool left = localStyle.get< bool >( "drop-shadow-left" );
      bool top = localStyle.get< bool >( "drop-shadow-top" );
      bool right = localStyle.get< bool >( "drop-shadow-right" );
      bool bottom = localStyle.get< bool >( "drop-shadow-bottom" );

      if( left ) {
        offset[ 0 ] = 5;
        offset[ 2 ] = -5;
      } else if( top ) {
        offset[ 1 ] = 5;
        offset[ 3 ] = -5;
      } else if( right ) {
        offset[ 2 ] = -5;
      } else if( bottom ) {
        offset[ 3 ] = -5;
      }

      onlyChild->setAllocation( glm::ivec4{ 0, 0, allocation[ 2 ], allocation[ 3 ] } + offset, false );
    }
  }

  void Pane::calculate() {
    if( children.size() ) {
      children[ 0 ]->calculate();
      requisition = children[ 0 ]->getRequisition();
    } else {
      requisition = glm::uvec2{ 1, 1 };
    }
  }

  void Pane::render( Graphics::Vector::Renderer& renderer ) {
    bool left = localStyle.get< bool >( "drop-shadow-left" );
    bool top = localStyle.get< bool >( "drop-shadow-top" );
    bool right = localStyle.get< bool >( "drop-shadow-right" );
    bool bottom = localStyle.get< bool >( "drop-shadow-bottom" );

    if( left ) {
      renderer.drawRect(
        { 5, 0, allocation[ 2 ] - 5, allocation[ 3 ] },
        localStyle.get< glm::uvec4 >( "background-color" )
      );

      renderer.drawLinearGradient(
        { 0, 0, 5, allocation[ 3 ] },
        { 5, allocation[ 3 ] / 2, 0, allocation[ 3 ] / 2 },
        { 0, 0, 0, 128 },
        { 0, 0, 0, 0 }
      );
    } else if( top ) {
      renderer.drawRect(
        { 0, 5, allocation[ 2 ], allocation[ 3 ] - 5 },
        localStyle.get< glm::uvec4 >( "background-color" )
      );

      renderer.drawLinearGradient(
        { 0, 0, allocation[ 2 ], 5 },
        { allocation[ 2 ] / 2, 5, allocation[ 2 ] / 2, 0 },
        { 0, 0, 0, 128 },
        { 0, 0, 0, 0 }
      );
    } else if( right ) {
      renderer.drawRect(
        { 0, 0, allocation[ 2 ] - 5, allocation[ 3 ] },
        localStyle.get< glm::uvec4 >( "background-color" )
      );

      renderer.drawLinearGradient(
        { allocation[ 2 ] - 5, 0, allocation[ 2 ], allocation[ 3 ] },
        { allocation[ 2 ] - 5, allocation[ 3 ] / 2, allocation[ 2 ], allocation[ 3 ] / 2 },
        { 0, 0, 0, 128 },
        { 0, 0, 0, 0 }
      );
    } else if( bottom ) {
      renderer.drawRect(
        { 0, 0, allocation[ 2 ], allocation[ 3 ] - 5 },
        localStyle.get< glm::uvec4 >( "background-color" )
      );

      renderer.drawLinearGradient(
        { 0, allocation[ 3 ] - 5, allocation[ 2 ], allocation[ 3 ] },
        { allocation[ 2 ] / 2, allocation[ 3 ] - 5, allocation[ 2 ] / 2, allocation[ 3 ] },
        { 0, 0, 0, 128 },
        { 0, 0, 0, 0 }
      );
    } else {
      // No drop shadow
      renderer.drawRect(
        { 0, 0, allocation[ 2 ], allocation[ 3 ] },
        localStyle.get< glm::uvec4 >( "background-color" )
      );
    }
  }

  void Pane::reflow() {
    if( auto parent = getParent() ) {
      parent->reflow();
    } else {
      Element::reflow();
    }
  }
}
