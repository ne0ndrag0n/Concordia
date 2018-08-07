#ifndef BB_WALL_MODEL_LOADER
#define BB_WALL_MODEL_LOADER

#include "graphics/scenegraph/modelloader/proceduralmodelloader.hpp"
#include "graphics/scenegraph/mesh/indexedmeshgenerator.hpp"
#include "graphics/scenegraph/mesh/texturedvertex.hpp"
#include "models/wallpaperregion.hpp"
#include "models/infrastructure.hpp"
#include "models/walljoint.hpp"
#include <SFML/Graphics/Image.hpp>
#include <glm/glm.hpp>
#include <optional>
#include <vector>
#include <memory>
#include <array>

namespace BlueBear::Graphics::SceneGraph { class Model; }
namespace BlueBear::Graphics::SceneGraph::ModelLoader {

  class WallModelLoader : public ProceduralModelLoader {
    using JointMap = std::vector< std::vector< Models::WallJoint > >;
    using Face = std::array< Mesh::TexturedVertex, 6 >;
    using FaceSet = std::vector< Face >;
    // Clockwise from lower right
    using WallpaperNeighborhood = std::array< std::optional< Models::WallpaperRegion >, 4 >;
    using VertexNeighborhood = std::array< std::optional< float >, 4 >;

    const std::vector< Models::Infrastructure::FloorLevel >& levels;

    sf::Image generateTexture( const Models::Infrastructure::FloorLevel& currentLevel );
    std::vector< std::vector< Models::WallJoint > > getJointMap(
      const glm::uvec2& dimensions,
      const std::vector< std::pair< glm::uvec2, glm::uvec2 > >& corners
    );

    FaceSet getSingleAxisFaceSet( const Models::WallJoint& joint, const glm::vec3& position, WallpaperNeighborhood wallpapers, VertexNeighborhood vertices );
    FaceSet getFaceSet( const glm::uvec2& position, const Models::Infrastructure::FloorLevel& floorLevel, const JointMap& jointMap );
    WallpaperNeighborhood getWallpaperNeighborhood( glm::ivec2 position, const Models::Infrastructure::FloorLevel& floorLevel );
    VertexNeighborhood getVertexNeighborhood( glm::ivec2 position, const Models::Infrastructure::FloorLevel& floorLevel );

  public:
    WallModelLoader( const std::vector< Models::Infrastructure::FloorLevel >& levels );

    std::shared_ptr< Model > get() override;
  };

}

#endif
