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
    using PlaneGroup = std::map< std::string, std::array< Mesh::TexturedVertex, 6 > >;

    struct Corner {
      struct {
        std::optional< Models::Sides > model;
        PlaneGroup stagedMesh;
      } horizontal;

      struct {
        std::optional< Models::Sides > model;
        PlaneGroup stagedMesh;
      } vertical;

      struct {
        std::optional< Models::Sides > model;
        PlaneGroup stagedMesh;
      } diagonal;

      struct {
        std::optional< Models::Sides > model;
        PlaneGroup stagedMesh;
      } reverseDiagonal;
    };

    Vector::Renderer& renderer;
    const glm::uvec2& dimensions;
    const std::vector< Models::WallSegment >& segments;
    std::vector< std::vector< Corner > > cornerMap;
    Utilities::TextureAtlas atlas;

    Corner* getCorner( const glm::ivec2& location );
    void updateStagedMesh( PlaneGroup& group, const glm::vec3& position, const glm::vec3& addValue );
    glm::vec3 indexToLocation( const glm::ivec2& position );

    bool adjustTopLeft( const glm::ivec2& index );
    bool adjustTopRight( const glm::ivec2& index );
    bool adjustBottomLeft( const glm::ivec2& index );
    bool adjustBottomRight( const glm::ivec2& index );

    void fixCorners( const glm::ivec2& startingIndex );
    void initTopTexture();
    void initCornerMap();

    void insertCornerMapSegment( const Models::WallSegment& segment );
    void insertIntoAtlas( const std::vector< Models::Sides >& sides, Utilities::TextureAtlas& atlas );

    std::array< Mesh::TexturedVertex, 6 > getPlane( const glm::vec3& origin, const glm::vec3& width, const glm::vec3& height, const std::string& wallpaperId );
    PlaneGroup sideToStagedMesh( const Models::Sides& sides, const glm::vec3& origin, const glm::vec3& width );
    void generateDeferredMeshes();
    std::shared_ptr< Model > generateModel();

  public:
    WallModelLoader( const glm::uvec2& dimensions, const std::vector< Models::WallSegment >& segments, Vector::Renderer& renderer );

    std::shared_ptr< Model > get() override;
  };

}


#endif
