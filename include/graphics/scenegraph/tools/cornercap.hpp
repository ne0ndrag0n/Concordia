#ifndef CORNER_CAP
#define CORNER_CAP

#include "graphics/scenegraph/tools/geometricsegment.hpp"
#include "graphics/scenegraph/tools/plane.hpp"
#include <optional>

namespace BlueBear::Graphics::SceneGraph::Tools {

  template< typename Vertex >
  struct CornerCap : public GeometricSegment {
    std::optional< Plane< Vertex > > top;
    std::optional< Plane< Vertex > > east;
    std::optional< Plane< Vertex > > south;

    bool isEmpty() const {
      return !top && !east && !south;
    }

    std::shared_ptr< Model > generateModel() override {
      // TODO !!
      return nullptr;
    }
  };

}

#endif
