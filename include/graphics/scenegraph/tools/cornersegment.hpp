#ifndef CORNER_SEGMENT
#define CORNER_SEGMENT

#include "graphics/scenegraph/tools/geometricsegment.hpp"
#include "graphics/scenegraph/tools/plane.hpp"
#include <optional>

namespace BlueBear::Graphics::SceneGraph{ class Model; }
namespace BlueBear::Graphics::SceneGraph::Tools {

  template< typename Vertex >
  struct CornerSegment : public GeometricSegment {
    std::optional< Plane< Vertex > > north;
    std::optional< Plane< Vertex > > eastSmall;
    std::optional< Plane< Vertex > > southSmall;
    std::optional< Plane< Vertex > > east;
    std::optional< Plane< Vertex > > south;
    std::optional< Plane< Vertex > > west;

    std::shared_ptr< Model > generateModel() override {
      // TODO !!
      return nullptr;
    }
  };

}

#endif
