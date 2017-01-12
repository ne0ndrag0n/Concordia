#ifndef CONCORDIA_BONE
#define CONCORDIA_BONE

#include <memory>
#include <glm/glm.hpp>

namespace BlueBear {
  namespace Graphics {

    template< typename T > struct Bone {
      std::shared_ptr< T > node;
      glm::mat4 inverseBindPose;
    };

  }
}


#endif
