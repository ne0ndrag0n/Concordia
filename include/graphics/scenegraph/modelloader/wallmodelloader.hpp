#ifndef WALL_MODEL_LOADER
#define WALL_MODEL_LOADER

#include "graphics/scenegraph/modelloader/proceduralmodelloader.hpp"
#include "graphics/scenegraph/mesh/facemeshgenerator.hpp"
#include "graphics/scenegraph/mesh/texturedvertex.hpp"
#include "graphics/utilities/textureatlas.hpp"
#include "models/wallsegment.hpp"
#include <vector>
#include <optional>

namespace BlueBear::Graphics { class Texture; }
namespace BlueBear::Graphics::Utilities{ class TextureAtlas; }
namespace BlueBear::Graphics::Vector{ class Renderer; }
namespace BlueBear::Graphics::SceneGraph::ModelLoader {

  class WallModelLoader : public ProceduralModelLoader {
    struct Corner {
      std::optional< Models::Sides > horizontal;
      std::optional< Models::Sides > vertical;
      std::optional< Models::Sides > diagonal;
      std::optional< Models::Sides > reverseDiagonal;
    };

    Vector::Renderer& renderer;
    const glm::uvec2& dimensions;
    const std::vector< Models::WallSegment >& segments;
    std::vector< std::vector< Corner > > cornerMap;
    std::shared_ptr< Texture > generatedTexture;
    Utilities::TextureAtlas atlas;

    Corner* getCorner( const glm::ivec2& location );

    void initTopTexture();
    void initCornerMap();
    void insertCornerMapSegment( const Models::WallSegment& segment );
    void insertIntoAtlas( const std::vector< Models::Sides >& sides, Utilities::TextureAtlas& atlas );
    std::array< Mesh::TexturedVertex, 6 > getPlane( const glm::vec3& origin, const glm::vec3& width, const glm::vec3& height, const std::string& wallpaperId );
    std::shared_ptr< Model > sideToModel( const Models::Sides& sides, const glm::vec3& origin, const glm::vec3& width );
    std::shared_ptr< Model > cornerToModel( const Corner& corner, const glm::vec3& topLeftCorner );
    std::shared_ptr< Model > generateModel();

  public:
    WallModelLoader( const glm::uvec2& dimensions, const std::vector< Models::WallSegment >& segments, Vector::Renderer& renderer );

    std::shared_ptr< Model > get() override;
  };

}


#endif
