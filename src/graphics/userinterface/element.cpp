#include "graphics/userinterface/element.hpp"
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
