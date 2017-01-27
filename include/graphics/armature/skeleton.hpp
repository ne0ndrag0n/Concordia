#ifndef CONCORDIA_BONE
#define CONCORDIA_BONE

#include <map>
#include <string>
#include <glm/glm.hpp>

namespace BlueBear {
  namespace Graphics {

    struct Bone {
      glm::mat4 transform;
      std::map< std::string, Bone > children;
    };
    using Skeleton = std::map< std::string, Bone >;

  }
}

#endif
