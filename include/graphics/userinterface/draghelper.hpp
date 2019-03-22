#ifndef DRAG_HELPER
#define DRAG_HELPER

#include <memory>
#include <glm/glm.hpp>

namespace BlueBear {
  namespace Device::Input {
    class Metadata;
  }

  namespace Graphics {
    namespace UserInterface {
      class Element;

      class DragHelper {
        std::shared_ptr< Element > target;
        glm::ivec2 offset;

      public:
        DragHelper( std::shared_ptr< Element > target, const glm::ivec2& offset );

        void update( Device::Input::Metadata event );
        void commit();
      };

    }
  }
}

#endif
