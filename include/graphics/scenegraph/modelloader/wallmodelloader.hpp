#ifndef BB_WALL_MODEL_LOADER
#define BB_WALL_MODEL_LOADER

#include "graphics/scenegraph/modelloader/proceduralmodelloader.hpp"
#include "graphics/scenegraph/mesh/indexedmeshgenerator.hpp"
#include "graphics/scenegraph/mesh/texturedvertex.hpp"
#include "models/infrastructure.hpp"
#include "models/walljoint.hpp"
#include <SFML/Graphics/Image.hpp>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <array>

namespace BlueBear::Graphics::SceneGraph { class Model; }
namespace BlueBear::Graphics::SceneGraph::ModelLoader {

  class WallModelLoader : public ProceduralModelLoader {
    using Face = std::array< Mesh::TexturedVertex, 6 >;

    const std::vector< Models::Infrastructure::FloorLevel >& levels;

    sf::Image generateTexture( const Models::Infrastructure::FloorLevel& currentLevel );
    std::vector< std::vector< Models::WallJoint > > getJointMap(
      const glm::uvec2& dimensions,
      const std::vector< std::pair< glm::uvec2, glm::uvec2 > >& corners
    );

    Face generateFace(
      const glm::vec3& lowerLeftCorner,
      const glm::vec2& dimensions
    );
    void addTrianglesForJoint(
      Mesh::IndexedMeshGenerator< Mesh::TexturedVertex >& generator,
      const Models::WallJoint& wallJoint,
      const glm::vec3& origin
    );

  public:
    WallModelLoader( const std::vector< Models::Infrastructure::FloorLevel >& levels );

    std::shared_ptr< Model > get() override;
  };

}

#endif
