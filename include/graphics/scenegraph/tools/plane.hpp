#ifndef BBPLANE
#define BBPLANE

#include "graphics/texture.hpp"
#include <array>

namespace BlueBear::Graphics::SceneGraph::Tools {

  template< typename Vertex >
  struct Plane {
    std::array< Vertex, 6 > vertexData;
    std::shared_ptr< Texture > texture;
  };

}

#endif
