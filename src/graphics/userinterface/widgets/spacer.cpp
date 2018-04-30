#include "graphics/userinterface/widgets/spacer.hpp"
#include "device/display/adapter/component/guicomponent.hpp"
#include "graphics/vector/renderer.hpp"
#include <glm/glm.hpp>

namespace BlueBear::Graphics::UserInterface::Widgets {

  Spacer::Spacer( const std::string& id, const std::vector< std::string >& classes ) : Element::Element( "Spacer", id, classes ) {}

  std::shared_ptr< Spacer > Spacer::create( const std::string& id, const std::vector< std::string >& classes ) {
    std::shared_ptr< Spacer > spacer( new Spacer( id, classes ) );

    return spacer;
  }

  bool Spacer::drawableDirty() {
    // Never dirty - don't ever draw.
    return false;
  }

  void Spacer::calculate() {
    requisition = glm::uvec2{ 1, 1 };
  }

  void Spacer::reflow() {
    if( auto parent = getParent() ) {
      parent->reflow();
    } else {
      Element::reflow();
    }
  }
}
