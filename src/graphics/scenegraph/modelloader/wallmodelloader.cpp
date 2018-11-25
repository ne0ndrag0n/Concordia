#include "graphics/scenegraph/modelloader/wallmodelloader.hpp"
#include "graphics/utilities/textureatlas.hpp"
#include "tools/cardinal.hpp"
#include "tools/utility.hpp"

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
    Mesh::FaceMeshGenerator< Mesh::TexturedVertex > generator;

    // Walk each line then check joint map to see if vertices need to be stretched to close corners
    for( const auto& segment : segments ) {
      // Add texture for front and back to cache
      insertIntoAtlas( segment.faces, atlas );

      Tools::Directional directional = Tools::getDirectional( segment.start, segment.end );
      glm::ivec2 cursor = directional.start;
      int originalDistance = Tools::Utility::distance( directional.start, directional.end );
      for( int distance = originalDistance; distance >= 0; distance-- ) {
        if( distance == originalDistance ) {
          // First one

        } else if( distance == 0 ) {
          // Last one

        } else {
          // Everything in between

        }

        cursor += directional.direction;
      }
    }

    return nullptr;
  }

}
