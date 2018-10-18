#ifndef SEGMENTEDCUBE
#define SEGMENTEDCUBE

#include "graphics/scenegraph/tools/geometricsegment.hpp"
#include "graphics/scenegraph/tools/plane.hpp"
#include <optional>

namespace BlueBear::Graphics::SceneGraph::Tools {

  template< typename Vertex >
  struct SegmentedCube : public GeometricSegment {
    std::optional< Plane< Vertex > > top;
    std::optional< Plane< Vertex > > north;
    std::optional< Plane< Vertex > > east;
    std::optional< Plane< Vertex > > west;
    std::optional< Plane< Vertex > > south;

    bool isEmpty() const {
      return !top && !north && !east && !west && !south;
    }

    std::shared_ptr< Model > generateModel() override {
      // TODO !!
      return nullptr;
    }
  };

}

#endif
