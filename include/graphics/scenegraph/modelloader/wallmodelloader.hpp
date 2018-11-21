#ifndef WALL_MODEL_LOADER
#define WALL_MODEL_LOADER

#include "graphics/scenegraph/modelloader/proceduralmodelloader.hpp"
#include "models/wallsegment.hpp"
#include <vector>

namespace BlueBear::Graphics::SceneGraph::ModelLoader {

  class WallModelLoader : public ProceduralModelLoader {
    const std::vector< Models::WallSegment >& segments;

  public:
    WallModelLoader( const std::vector< Models::WallSegment >& segments );

    std::shared_ptr< Model > get() override;
  };

}


#endif
