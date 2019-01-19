#ifndef WALL_MODEL_LOADER
#define WALL_MODEL_LOADER

#include "graphics/scenegraph/modelloader/proceduralmodelloader.hpp"
#include "graphics/scenegraph/mesh/facemeshgenerator.hpp"
#include "graphics/scenegraph/mesh/texturedvertex.hpp"
#include "models/wallsegment.hpp"
#include <vector>
#include <optional>

namespace BlueBear::Graphics::Utilities{ class TextureAtlas; }
namespace BlueBear::Graphics::SceneGraph::ModelLoader {

  class WallModelLoader : public ProceduralModelLoader {
    struct Corner {
      std::optional< Models::Sides > horizontal;
      std::optional< Models::Sides > vertical;
      std::optional< Models::Sides > diagonal;
      std::optional< Models::Sides > reverseDiagonal;
    };

    const glm::uvec2& dimensions;
    const std::vector< Models::WallSegment >& segments;
    std::vector< std::vector< Corner > > cornerMap;

    Corner* getCorner( const glm::ivec2& location );

    void initCornerMap();
    void insertCornerMapSegment( const Models::WallSegment& segment );
    void fixCorners();
    void insertIntoAtlas( const std::vector< Models::Sides >& sides, Utilities::TextureAtlas& atlas );

  public:
    WallModelLoader( const glm::uvec2& dimensions, const std::vector< Models::WallSegment >& segments );

    std::shared_ptr< Model > get() override;
  };

}


#endif
