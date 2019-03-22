#include "graphics/userinterface/draghelper.hpp"
#include "graphics/userinterface/element.hpp"
#include "device/input/input.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {

      DragHelper::DragHelper( std::shared_ptr< Element > target, const glm::ivec2& offset ) : target( target ), offset( offset ) {}

      void DragHelper::update( Device::Input::Metadata event ) {
        glm::ivec2 newPos{ event.mouseLocation.x - offset.x, event.mouseLocation.y - offset.y };

        auto allocation = target->getAllocation();
        allocation.x = newPos.x;
        allocation.y = newPos.y;

        target->setAllocation( allocation, false );

        target->getPropertyList().set< int >( "left", allocation.x, false );
        target->getPropertyList().set< int >( "top", allocation.y, false );
      }

      void DragHelper::commit() {
        if( std::shared_ptr< Element > parent = target->getParent() ) {
          parent->paint();
        }
      }

    }
  }
}
