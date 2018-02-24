#include "graphics/userinterface/element.hpp"
#include "device/display/adapter/component/guicomponent.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {

      Element::Element( const std::string& tag, const std::string& id, const std::vector< std::string >& classes ) : tag( tag ), id( id ), classes( classes ) {}

      Element::~Element() {}

      void Element::reflow( Device::Display::Adapter::Component::GuiComponent& manager ) {
        render( manager.getVectorRenderer() );
        positionAndSizeChildren();

        for( std::shared_ptr< Element > child : children ) {
          child->reflow( manager );
        }
      }

    }
  }
}
