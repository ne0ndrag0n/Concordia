#include "graphics/userinterface/widgets/scroll.hpp"
#include "device/display/adapter/component/guicomponent.hpp"
#include "device/input/input.hpp"
#include "graphics/vector/renderer.hpp"

#include "log.hpp"

namespace BlueBear::Graphics::UserInterface::Widgets {

  Scroll::Scroll( const std::string& id, const std::vector< std::string >& classes ) : Element::Element( "Scroll", id, classes ) {}

  std::shared_ptr< Scroll > Scroll::create( const std::string& id, const std::vector< std::string >& classes ) {
    std::shared_ptr< Scroll > scroll( new Scroll( id, classes ) );

    // Add shadow element
    auto overlay = ScrollOverlay::create();
    scroll->addChild( overlay, false );

    return scroll;
  }

  bool Scroll::addedChildren() {
    return children.size() >= 2;
  }

  bool Scroll::getXScrollSet() {
    return localStyle.get< bool >( "scrollbar-x" );
  }

  bool Scroll::getYScrollSet() {
    return localStyle.get< bool >( "scrollbar-y" );
  }

  bool Scroll::getXVisible() {
    return ( getXScrollSet() && getXRatio() < 1.0f );
  }

  bool Scroll::getYVisible() {
    return ( getYScrollSet() && getYRatio() < 1.0f );
  }

  float Scroll::getXRatio() {
    return ( ( float ) allocation[ 2 ] / ( float ) getFinalRequisition( getSingleContainer() ).x );
  }

  float Scroll::getYRatio() {
    return ( ( float ) allocation[ 3 ] / ( float ) getFinalRequisition( getSingleContainer() ).y );
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

  std::shared_ptr< Element > Scroll::getSingleContainer() {
    return children[ 1 ];
  }

  void Scroll::updateX( int x ) {
    if( getXScrollSet() ) {
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
    if( getYScrollSet() ) {
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
    std::static_pointer_cast< ScrollOverlay >( children[ 0 ] )->regenerate();
  }

  glm::uvec2 Scroll::getFinalRequisition( std::shared_ptr< Element > prospect ) {
    int width = prospect->getPropertyList().get< int >( "width" );
    int height = prospect->getPropertyList().get< int >( "height" );

    return glm::uvec2{
      valueIsLiteral( width ) ? width : prospect->getRequisition().x,
      valueIsLiteral( height ) ? height : prospect->getRequisition().y
    };
  }

  void Scroll::positionAndSizeChildren() {
    children[ 0 ]->setAllocation( allocation, false );

    if( addedChildren() ) {
      std::shared_ptr< Element > onlyChild = getSingleContainer();
      glm::uvec2 finalRequisition = getFinalRequisition( onlyChild );

      onlyChild->setAllocation( {
        -( scrollX * finalRequisition.x ),
        -( scrollY * finalRequisition.y ),
        finalRequisition.x,
        finalRequisition.y
      }, false );
    }
  }

  void Scroll::setChildrenZOrder() {
    children[ 0 ]->setLocalZOrder( 1 );
    if( addedChildren() ) {
      getSingleContainer()->setLocalZOrder( 2 );
    }
  }

  void Scroll::calculate() {
    if( addedChildren() ) {
      getSingleContainer()->calculate();
    }

    glm::uvec2 differential{ getXScrollSet() ? 10 : 0, getYScrollSet() ? 10 : 0 };
    requisition = glm::uvec2{ 1, 1 } + differential;
  }

  void Scroll::reflow( bool selectorsInvalidated ) {
    if( auto parent = getParent() ) {
      parent->reflow( selectorsInvalidated );
    } else {
      Element::reflow( selectorsInvalidated );
    }
  }

  glm::vec2 Scroll::getScrollAttributes() {
    return { scrollX, scrollY };
  }

  ScrollOverlay::ScrollOverlay() : Element::Element( "ScrollOverlay", "", { "-bb-shadow-scrolloverlay" } ) {
    shadow = true;
    eventBundle.registerInputEvent( "mouse-down", std::bind( &ScrollOverlay::onMouseDown, this, std::placeholders::_1 ) );
  }

  std::shared_ptr< ScrollOverlay > ScrollOverlay::create() {
    return std::shared_ptr< ScrollOverlay >( new ScrollOverlay() );
  }

  void ScrollOverlay::onMouseDown( Device::Input::Metadata event ) {
    auto parent = std::static_pointer_cast< Scroll >( getParent() );
    auto relative = toRelative( event.mouseLocation );
    auto allocation = parent->getAllocation();

    if( parent->getXVisible() ) {
      if( relative.x >= 0 && relative.y >= allocation[ 3 ] - 10 && relative.x <= parent->getXGutter() && relative.y <= allocation[ 3 ] ) {
        parent->updateX( relative.x );

        manager->setupBlockingGlobalEvent( "mouse-moved", [ parent ]( Device::Input::Metadata e ) {
          parent->updateX( parent->toRelative( e.mouseLocation ).x );
        } );

        manager->setupBlockingGlobalEvent( "mouse-up", [ parent ]( Device::Input::Metadata e ) {
          manager->unregisterBlockingGlobalEvent( "mouse-moved" );
          manager->unregisterBlockingGlobalEvent( "mouse-up" );
        } );
      }
    }

    if( parent->getYVisible() ) {
      if( relative.x >= allocation[ 2 ] - 10 && relative.y >= 0 && relative.x <= allocation[ 2 ] && relative.y <= parent->getYGutter() ) {
        parent->updateY( relative.y );

        manager->setupBlockingGlobalEvent( "mouse-moved", [ parent ]( Device::Input::Metadata e ) {
          parent->updateY( parent->toRelative( e.mouseLocation ).y );
        } );

        manager->setupBlockingGlobalEvent( "mouse-up", [ parent ]( Device::Input::Metadata e ) {
          manager->unregisterBlockingGlobalEvent( "mouse-moved" );
          manager->unregisterBlockingGlobalEvent( "mouse-up" );
        } );
      }
    }
  }

  void ScrollOverlay::regenerate() {
    forceDirty = true;
    generateDrawable();
  }

  void ScrollOverlay::calculate() {
    // Nothing to calculate - height and width are known when positioned
    requisition = { 0, 0 };
  }

  bool ScrollOverlay::drawableDirty() {
    if( forceDirty ) {
      forceDirty = false;
      return true;
    }

    return Element::drawableDirty();
  }

  void ScrollOverlay::reflow( bool selectorsInvalidated ) {
    if( auto parent = getParent() ) {
      parent->reflow( selectorsInvalidated );
    } else {
      Element::reflow( selectorsInvalidated );
    }
  }

  void ScrollOverlay::render( Graphics::Vector::Renderer& renderer ) {
    auto parent = std::static_pointer_cast< Scroll >( getParent() );

    auto attributes = parent->getScrollAttributes();
    float scrollX = attributes.x;
    float scrollY = attributes.y;

    auto allocation = parent->getAllocation();

    if( parent->addedChildren() ) {
      if( parent->getXVisible() ) {
        // Gutter
        renderer.drawRect(
          { 0, allocation[ 3 ] - 10, parent->getXGutter(), allocation[ 3 ] },
          parent->getPropertyList().get< glm::uvec4 >( "background-color" )
        );

        int xSpace = parent->getXSpace();
        int barSize = xSpace * parent->getXRatio();

        // Bar
        renderer.drawRect(
          { 1 + ( scrollX * xSpace ), allocation[ 3 ] - 9, 1 + ( scrollX * xSpace ) + barSize, allocation[ 3 ] - 1 },
          parent->getPropertyList().get< glm::uvec4 >( "color" )
        );
      }

      if( parent->getYVisible() ) {
        // Gutter
        renderer.drawRect(
          { allocation[ 2 ] - 10, 0, allocation[ 2 ], parent->getYGutter() },
          parent->getPropertyList().get< glm::uvec4 >( "background-color" )
        );

        int ySpace = parent->getYSpace();
        int barSize = ySpace * parent->getYRatio();

        // Bar
        renderer.drawRect(
          { allocation[ 2 ] - 9, 1 + ( scrollY * ySpace ), allocation[ 2 ] - 1, 1 + ( scrollY * ySpace ) + barSize },
          parent->getPropertyList().get< glm::uvec4 >( "color" )
        );
      }
    }
  }

}
