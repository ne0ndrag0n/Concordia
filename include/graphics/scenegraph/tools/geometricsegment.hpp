#ifndef GEOMETRIC_SEGMENT
#define GEOMETRIC_SEGMENT

#include <memory>

namespace BlueBear::Graphics::SceneGraph{ class Model; }
namespace BlueBear::Graphics::SceneGraph::Tools {

  struct GeometricSegment {
    virtual std::shared_ptr< Model > generateModel() = 0;
  };
}

#endif
