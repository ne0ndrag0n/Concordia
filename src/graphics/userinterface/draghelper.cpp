#include "graphics/userinterface/draghelper.hpp"
#include "graphics/userinterface/element.hpp"
#include "device/input/input.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {

      DragHelper::DragHelper( std::shared_ptr< Element > target, const glm::ivec2& offset ) : target( target ), offset( offset ) {}

      void DragHelper::update( Device::Input::Metadata event ) {
        target->getPropertyList().set< int >( "left", ( int ) event.mouseLocation.x - offset.x, false );
        target->getPropertyList().set< int >( "top", ( int ) event.mouseLocation.y - offset.y, false );

        if( std::shared_ptr< Element > parent = target->getParent() ) {
          parent->paint();
        }
      }

    }
  }
}
