#ifndef WALL_MODEL_LOADER
#define WALL_MODEL_LOADER

#include "graphics/scenegraph/modelloader/proceduralmodelloader.hpp"
#include "graphics/scenegraph/mesh/indexedmeshgenerator.hpp"
#include "graphics/scenegraph/mesh/texturedvertex.hpp"
#include "graphics/utilities/textureatlas.hpp"
#include "graphics/scenegraph/drawable.hpp"
#include "models/infrastructure.hpp"
#include "models/wallsegment.hpp"
#include <vector>
#include <optional>

namespace BlueBear::Graphics { class Texture; }
namespace BlueBear::Graphics::Utilities{ class TextureAtlas; class ShaderManager; }
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

    glm::ivec2 dimensions;
    int currentLevel = 0;
    const std::vector< Models::Infrastructure::FloorLevel >& floorLevels;
    std::vector< std::vector< Corner > > cornerMap;
    std::shared_ptr< Shader > shader;
    Utilities::TextureAtlas atlas;

    Corner* getCorner( const glm::ivec2& location );
    glm::vec3 getPositionById( const PlaneGroup& group, const std::string& face, int index ) const;
    void updateStagedMesh( PlaneGroup& group, const glm::vec3& position, const glm::vec3& addValue, bool replace = false );
    glm::vec3 indexToLocation( const glm::ivec2& position );

    bool adjustTopLeft( const glm::ivec2& index );
    bool adjustTopRight( const glm::ivec2& index );
    bool adjustBottomLeft( const glm::ivec2& index );
    bool adjustBottomRight( const glm::ivec2& index );
    bool adjustDiagonalTop12( const glm::ivec2& index );
    bool adjustDiagonalTop1( const glm::ivec2& index );
    bool adjustDiagonalTop3( const glm::ivec2& index );
    bool adjustDiagonalTop6( const glm::ivec2& index );

    void fixCorners( const glm::ivec2& startingIndex );
    void initTopTexture( Vector::Renderer& renderer );
    void initCornerMap();

    void insertCornerMapSegment( const Models::WallSegment& segment );
    void insertIntoAtlas( const std::vector< Models::Sides >& sides, Utilities::TextureAtlas& atlas );

    std::array< Mesh::TexturedVertex, 6 > getPlane( const glm::vec3& origin, const glm::vec3& width, const glm::vec3& height, const glm::vec3& normal, const std::string& wallpaperId );
    PlaneGroup sideToStagedMesh( const Models::Sides& sides, const glm::vec3& origin, const glm::vec3& width, float thickness = 0.1f );
    void generateDeferredMeshes();
    void addToGenerator( Mesh::IndexedMeshGenerator< Mesh::TexturedVertex >& generator, const PlaneGroup& planeGroup );
    std::shared_ptr< Model > getLevel();

  public:
    WallModelLoader( const std::vector< Models::Infrastructure::FloorLevel >& floorLevels, Vector::Renderer& renderer, Utilities::ShaderManager& shaderManager );

    std::shared_ptr< Model > get() override;
  };

}


#endif
