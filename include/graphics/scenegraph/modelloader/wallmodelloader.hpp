#ifndef BB_WALL_MODEL_LOADER
#define BB_WALL_MODEL_LOADER

#include "graphics/scenegraph/modelloader/proceduralmodelloader.hpp"
#include "graphics/scenegraph/tools/segmentedcube.hpp"
#include "graphics/scenegraph/mesh/texturedvertex.hpp"
#include "graphics/scenegraph/tools/plane.hpp"
#include "models/wallpaperregion.hpp"
#include "models/infrastructure.hpp"
#include <glm/glm.hpp>
#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

namespace BlueBear::Graphics { class Texture; }
namespace BlueBear::Graphics::SceneGraph { class Model; }
namespace BlueBear::Graphics::SceneGraph::ModelLoader {

  class WallModelLoader : public ProceduralModelLoader {
    using CubeSegmentGrid = std::vector< std::vector< std::optional< Tools::SegmentedCube< Mesh::TexturedVertex > > > >;
    using RegionModelGrid = std::vector< std::vector< Models::WallpaperRegion > >;

    const std::vector< Models::Infrastructure::FloorLevel >& levels;
    std::unordered_map< std::string, std::shared_ptr< Texture > > textures;
    std::shared_ptr< Shader > rootShader = nullptr;

    std::array< Mesh::TexturedVertex, 6 > getPlane( const glm::vec3& origin, const glm::vec3& horizontalDirection, const glm::vec3& verticalDirection );
    Tools::SegmentedCube< Mesh::TexturedVertex > getSegmentedCubeRegular( const glm::vec3& origin, const glm::vec3& dimensions );
    std::optional< Models::WallpaperRegion > getWallpaperRegion( int x, int y, const RegionModelGrid& grid );
    std::shared_ptr< Texture > getTexture( const std::string& key, const sf::Image& imageToLoad );
    CubeSegmentGrid regionsToSegments( const RegionModelGrid& wallpaperGrid );
    std::shared_ptr< Model > generateWallsForStory( const CubeSegmentGrid& segmentGrid );

  public:
    WallModelLoader( const std::vector< Models::Infrastructure::FloorLevel >& levels );

    virtual std::shared_ptr< Model > get() override;
  };

}

#endif
