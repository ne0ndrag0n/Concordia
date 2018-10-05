#include <array>

namespace BlueBear::Graphics::SceneGraph::Tools {

  template< typename Vertex >
  using Plane = std::array< Vertex, 6 >;

}
