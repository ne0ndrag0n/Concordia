#include "graphics/scenegraph/modelloader/wallmodelloader.hpp"
#include "graphics/scenegraph/mesh/facemeshgenerator.hpp"
#include "graphics/utilities/textureatlas.hpp"

namespace BlueBear::Graphics::SceneGraph::ModelLoader {

  WallModelLoader::WallModelLoader( const glm::uvec2& dimensions, const std::vector< Models::WallSegment >& segments )
    : dimensions( dimensions ), segments( segments ), jointMap( Models::WallJoint::createFromList( dimensions.x, dimensions.y, segments ) ) {}

  void WallModelLoader::fixCorners() {

  }

  void WallModelLoader::insertIntoAtlas( const std::vector< Models::Sides >& sides, Utilities::TextureAtlas& atlas ) {
    for( const auto& side : sides ) {
      const auto& [ frontWallpaperId, frontWallpaper ] = side.front;
      const auto& [ backWallpaperId, backWallpaper ] = side.back;

      atlas.addTexture( frontWallpaperId, frontWallpaper.surface );
      atlas.addTexture( backWallpaperId, backWallpaper.surface );
    }
  }

  std::shared_ptr< Model > WallModelLoader::get() {
    Utilities::TextureAtlas atlas;

    // Walk each line then check joint map to see if vertices need to be stretched to close corners
    for( const auto& segment : segments ) {
      // Add texture for front and back to cache
      insertIntoAtlas( segment.faces, atlas );

      // TODO
    }

    return nullptr;
  }

}
