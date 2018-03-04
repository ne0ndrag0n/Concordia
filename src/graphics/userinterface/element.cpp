#include "graphics/userinterface/element.hpp"
#include "graphics/userinterface/propertylist.hpp"
#include "device/display/adapter/component/guicomponent.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {

      Element::Element( const std::string& tag, const std::string& id, const std::vector< std::string >& classes ) : tag( tag ), id( id ), classes( classes ) {}

      Element::~Element() {}

      void Element::setAllocation( const glm::uvec4& allocation ) {
        this->allocation = allocation;
      }

      PropertyList& Element::getPropertyList() {
        return localStyle;
      }

      glm::uvec2 Element::getRequisition() {
        return requisition;
      }

      void Element::positionAndSizeChildren() {
        // abstract !!
      }

      bool Element::valueIsLiteral( int r ) {
        return ( Requisition ) r != Requisition::AUTO &&
          ( Requisition ) r != Requisition::NONE &&
          ( Requisition ) r != Requisition::FILL_PARENT;
      }

      glm::uvec2 Element::bindCalculations( glm::ivec2 total ) {
        // Bound by min-height and max-height
        glm::ivec2 minima{ localStyle.get< int >( "min-width" ), localStyle.get< int >( "min-height" ) };
        glm::ivec2 maxima{ localStyle.get< int >( "max-width" ), localStyle.get< int >( "max-height" ) };

        if( valueIsLiteral( minima.x ) ) { total.x = std::max( minima.x, total.x ); }
        if( valueIsLiteral( minima.y ) ) { total.y = std::max( minima.y, total.y ); }

        if( valueIsLiteral( maxima.x ) ) { total.x = std::min( total.x, maxima.x ); }
        if( valueIsLiteral( maxima.y ) ) { total.x = std::min( total.y, maxima.y ); }

        return total;
      }

      void Element::reflow( Device::Display::Adapter::Component::GuiComponent& manager ) {
        // Render myself, since I've already been positioned and sized
        render( manager );
        positionAndSizeChildren();

        for( std::shared_ptr< Element > child : children ) {
          child->reflow( manager );
        }
      }

    }
  }
}
