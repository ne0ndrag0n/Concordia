#include "graphics/userinterface/widgets/scroll.hpp"
#include "device/display/adapter/component/guicomponent.hpp"
#include "device/input/input.hpp"
#include "graphics/vector/renderer.hpp"

#include "log.hpp"

namespace BlueBear::Graphics::UserInterface::Widgets {

  Scroll::Scroll( const std::string& id, const std::vector< std::string >& classes ) : Element::Element( "Scroll", id, classes ) {
    eventBundle.registerInputEvent( "mouse-down", std::bind( &Scroll::onMouseDown, this, std::placeholders::_1 ) );
  }

  std::shared_ptr< Scroll > Scroll::create( const std::string& id, const std::vector< std::string >& classes ) {
    std::shared_ptr< Scroll > scroll( new Scroll( id, classes ) );

    return scroll;
  }

  bool Scroll::getXVisible() {
    return ( localStyle.get< bool >( "scrollbar-x" ) && getXRatio() < 1.0f );
  }

  bool Scroll::getYVisible() {
    return ( localStyle.get< bool >( "scrollbar-y" ) && getYRatio() < 1.0f );
  }

  float Scroll::getXRatio() {
    return ( ( float ) allocation[ 2 ] / ( float ) getFinalRequisition( children[ 0 ] ).x );
  }

  float Scroll::getYRatio() {
    return ( ( float ) allocation[ 3 ] / ( float ) getFinalRequisition( children[ 0 ] ).y );
  }

  int Scroll::getXGutter() {
    return allocation[ 2 ] - ( getYVisible() ? 10 : 0 );
  }

  int Scroll::getYGutter() {
    return allocation[ 3 ] - ( getXVisible() ? 10 : 0 );
  }

  int Scroll::getXSpace() {
    return getXGutter() - 2;
  }

  int Scroll::getYSpace() {
    return getYGutter() - 2;
  }

  void Scroll::onMouseDown( Device::Input::Metadata event ) {
    auto relative = toRelative( event.mouseLocation );

    if( getXVisible() ) {
      if( relative.x >= 0 && relative.y >= allocation[ 3 ] - 10 && relative.x <= getXGutter() && relative.y <= allocation[ 3 ] ) {
        updateX( relative.x );

        manager->setupBlockingGlobalEvent( "mouse-moved", [ & ]( Device::Input::Metadata e ) {
          updateX( toRelative( e.mouseLocation ).x );
        } );

        manager->setupBlockingGlobalEvent( "mouse-up", [ & ]( Device::Input::Metadata e ) {
          manager->unregisterBlockingGlobalEvent( "mouse-moved" );
          manager->unregisterBlockingGlobalEvent( "mouse-up" );
        } );
      }
    }

    if( getYVisible() ) {
      if( relative.x >= allocation[ 2 ] - 10 && relative.y >= 0 && relative.x <= allocation[ 2 ] && relative.y <= getYGutter() ) {
        updateY( relative.y );

        manager->setupBlockingGlobalEvent( "mouse-moved", [ & ]( Device::Input::Metadata e ) {
          updateY( toRelative( e.mouseLocation ).y );
        } );

        manager->setupBlockingGlobalEvent( "mouse-up", [ & ]( Device::Input::Metadata e ) {
          manager->unregisterBlockingGlobalEvent( "mouse-moved" );
          manager->unregisterBlockingGlobalEvent( "mouse-up" );
        } );
      }
    }
  }

  void Scroll::updateX( int x ) {
    if( localStyle.get< bool >( "scrollbar-x" ) ) {
      int xSpace = getXSpace();
      int boxWidth = xSpace * getXRatio();
      int newX = x - ( boxWidth / 2 );

      // newX must be >= 1 and <= allocation[ 2 ] - 1
      newX = std::max( newX, 0 );
      int gutterWidth = getXGutter();
      if( ( newX + boxWidth ) > gutterWidth - 1 ) {
        newX = gutterWidth - 2 - boxWidth;
      }

      // Calculate new scrollX proportion
      float newScrollX = ( float ) newX / ( float ) xSpace;
      scrollX = newScrollX;
      partialReflow();
    }
  }

  void Scroll::updateY( int y ) {
    if( localStyle.get< bool >( "scrollbar-y" ) ) {
      int ySpace = getYSpace();
      int boxWidth = ySpace * getYRatio();
      int newY = y - ( boxWidth / 2 );

      newY = std::max( newY, 0 );
      int gutterWidth = getYGutter();
      if( ( newY + boxWidth ) > gutterWidth - 1 ) {
        newY = gutterWidth - 2 - boxWidth;
      }

      float newScrollY = ( float ) newY / ( float ) ySpace;
      scrollY = newScrollY;
      partialReflow();
    }
  }

  /**
   * Reflow that re-renders local element and only repositions child elements - king-size reflow is way too slow. Only the scrollbars
   * need to be updated and the *immediate* children repositioned.
   */
  void Scroll::partialReflow() {
    positionAndSizeChildren();
    forceDirty = true;
    generateDrawable();
  }

  glm::uvec2 Scroll::getFinalRequisition( std::shared_ptr< Element > prospect ) {
    int width = prospect->getPropertyList().get< int >( "width" );
    int height = prospect->getPropertyList().get< int >( "height" );

    return glm::uvec2{
      valueIsLiteral( width ) ? width : prospect->getRequisition().x,
      valueIsLiteral( height ) ? height : prospect->getRequisition().y
    };
  }

  bool Scroll::drawableDirty() {
    if( forceDirty ) {
      forceDirty = false;
      return true;
    }

    return Element::drawableDirty();
  }

  void Scroll::positionAndSizeChildren() {
    if( children.size() ) {
      std::shared_ptr< Element > onlyChild = children[ 0 ];
      glm::uvec2 finalRequisition = getFinalRequisition( onlyChild );

      finalRequisition += glm::uvec2{ 30.0f, 15.0f };

      onlyChild->setAllocation( {
        -( scrollX * finalRequisition.x ),
        -( scrollY * finalRequisition.y ),
        finalRequisition.x,
        finalRequisition.y
      }, false );
    }
  }

  void Scroll::render( Graphics::Vector::Renderer& renderer ) {
    if( children.size() ) {
      std::shared_ptr< Element > onlyChild = children[ 0 ];

      if( getXVisible() ) {
        // Gutter
        renderer.drawRect(
          { 0, allocation[ 3 ] - 10, getXGutter(), allocation[ 3 ] },
          localStyle.get< glm::uvec4 >( "background-color" )
        );

        int xSpace = getXSpace();
        int barSize = xSpace * getXRatio();

        // Bar
        renderer.drawRect(
          { 1 + ( scrollX * xSpace ), allocation[ 3 ] - 9, 1 + ( scrollX * xSpace ) + barSize, allocation[ 3 ] - 1 },
          localStyle.get< glm::uvec4 >( "color" )
        );
      }

      if( getYVisible() ) {
        // Gutter
        renderer.drawRect(
          { allocation[ 2 ] - 10, 0, allocation[ 2 ], getYGutter() },
          localStyle.get< glm::uvec4 >( "background-color" )
        );

        int ySpace = getYSpace();
        int barSize = ySpace * getYRatio();

        // Bar
        renderer.drawRect(
          { allocation[ 2 ] - 9, 1 + ( scrollY * ySpace ), allocation[ 2 ] - 1, 1 + ( scrollY * ySpace ) + barSize },
          localStyle.get< glm::uvec4 >( "color" )
        );
      }
    }
  }

  void Scroll::calculate() {
    if( children.size() ) {
      children[ 0 ]->calculate();
    }

    glm::uvec2 differential{ localStyle.get< bool >( "scrollbar-x" ) ? 10 : 0, localStyle.get< bool >( "scrollbar-y" ) ? 10 : 0 };
    requisition = glm::uvec2{ 1, 1 } + differential;
  }

  void Scroll::reflow( bool selectorsInvalidated ) {
    if( auto parent = getParent() ) {
      parent->reflow( selectorsInvalidated );
    } else {
      Element::reflow( selectorsInvalidated );
    }
  }

  glm::vec4 Scroll::computeScissor( const glm::vec4& parentScissor, const glm::ivec2& absolutePosition ) {
    // parent: vec4(5.000000, 928.000000, 1142.000000, -64.000000)
    // child: vec4(5.000000, 938.000000, 1147.000000, -74.000000)
    // parent: vec4(5.000000, 478.000000, 1142.000000, 381.000000)
    // child: vec4(5.000000, 488.000000, 1142.000000, 371.000000)
    glm::vec4 computedScissor = Element::computeScissor( parentScissor, absolutePosition );

    if( getYVisible() ) {
      computedScissor[ 2 ] -= 10.0f;
    }

    if( getXVisible() ) {
      computedScissor[ 1 ] += 10.0f;
    }

    return computedScissor;
  }

}
