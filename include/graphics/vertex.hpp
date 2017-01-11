#ifndef CONCORDIA_VERTEX
#define CONCORDIA_VERTEX

#include <glm/glm.hpp>

namespace BlueBear {
  namespace Graphics {

    struct Vertex {
      glm::vec3 position;
      glm::vec3 normal;
      glm::vec2 textureCoordinates;
    };

  }
}


#endif
