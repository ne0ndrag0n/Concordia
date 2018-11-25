#ifndef WALL_MODEL_LOADER
#define WALL_MODEL_LOADER

#include "graphics/scenegraph/modelloader/proceduralmodelloader.hpp"
#include "models/wallsegment.hpp"
#include "models/walljoint.hpp"
#include <vector>

namespace BlueBear::Graphics::Utilities{ class TextureAtlas; }
namespace BlueBear::Graphics::SceneGraph::ModelLoader {

  class WallModelLoader : public ProceduralModelLoader {
    const glm::uvec2& dimensions;
    const std::vector< Models::WallSegment >& segments;
    std::vector< std::vector< Models::WallJoint > > jointMap;

    void fixCorners();
    void insertIntoAtlas( const std::vector< Models::Sides >& sides, Utilities::TextureAtlas& atlas );

  public:
    WallModelLoader( const glm::uvec2& dimensions, const std::vector< Models::WallSegment >& segments );

    std::shared_ptr< Model > get() override;
  };

}


#endif
