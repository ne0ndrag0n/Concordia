#include "graphics/scenegraph/modelloader/wallmodelloader.hpp"
#include "graphics/utilities/textureatlas.hpp"
#include "tools/utility.hpp"

namespace BlueBear::Graphics::SceneGraph::ModelLoader {

  WallModelLoader::WallModelLoader( const glm::uvec2& dimension, const std::vector< Models::WallSegment >& segments )
    : dimensions( dimensions ), segments( segments ) {
      initCornerMap();
    }

  void WallModelLoader::initCornerMap() {
    cornerMap.reserve( dimensions.y );
    for( auto& xLevel : cornerMap ) {
      xLevel.reserve( dimensions.x );
    }
  }

  WallModelLoader::Corner* WallModelLoader::getCorner( const glm::ivec2& location ) {
    if( location.x < 0 || location.y < 0 ) {
      return nullptr;
    }

    if( location.x >= dimensions.x || location.y >= dimensions.y ) {
      return nullptr;
    }

    return &cornerMap[ location.y ][ location.x ];
  }

  void WallModelLoader::insertCornerMapSegment( const Models::WallSegment& segment ) {
    glm::ivec2 direction = glm::normalize( glm::vec2( segment.end - segment.start ) );
    glm::ivec2 cursor = segment.start;
    int distance = std::abs( glm::distance( glm::vec2( segment.start ), glm::vec2( segment.end ) ) );

    for( int i = 0; i != distance; i++ ) {
      switch( direction.x ) {
        case -1: {
          switch( direction.y ) {
            case -1: {
              //( -1, -1 ) case
              if( Corner* corner = getCorner( cursor - glm::ivec2{ -1, -1 } ) ) {
                corner->diagonal = segment.faces[ i ];
              } else {
                // Advance to termination event
                i = distance;
              }

              cursor += direction;
              continue;
            }
            case 0: {
              //( -1, 0 ) case
              if( Corner* corner = getCorner( cursor - glm::ivec2{ -1, 0 } ) ) {
                corner->horizontal = segment.faces[ i ];
              } else {
                // Advance to termination event
                i = distance;
              }

              cursor += direction;
              continue;
            }
            case 1: {
              //( -1, 1 ) case
              // This is not a mistake! We need to reuse the cell to the left and use its reverseDiagonal
              if( Corner* corner = getCorner( cursor - glm::ivec2{ -1, 0 } ) ) {
                corner->reverseDiagonal = segment.faces[ i ];
              } else {
                // Advance to termination event
                i = distance;
              }

              cursor += direction;
              continue;
            }
          }
        }
        case 0: {
          switch( direction.y ) {
            case -1: {
              //( 0, -1 ) case
              if( Corner* corner = getCorner( cursor - glm::ivec2{ 0, -1 } ) ) {
                corner->vertical = segment.faces[ i ];
              } else {
                // Advance to termination event
                i = distance;
              }

              cursor += direction;
              continue;
            }
            case 1: {
              //( 0, 1 ) case
              if( Corner* corner = getCorner( cursor ) ) {
                corner->vertical = segment.faces[ i ];
              } else {
                // Advance to termination event
                i = distance;
              }

              cursor += direction;
              continue;
            }
          }
        }
        case 1: {
          switch( direction.y ) {
            case -1: {
              //( 1, -1 ) case
              if( Corner* corner = getCorner( cursor - glm::ivec2{ 0, -1 } ) ) {
                corner->reverseDiagonal = segment.faces[ i ];
              } else {
                // Advance to termination event
                i = distance;
              }

              cursor += direction;
              continue;
            }
            case 0: {
              //( 1, 0 ) case
              if( Corner* corner = getCorner( cursor ) ) {
                corner->horizontal = segment.faces[ i ];
              } else {
                // Advance to termination event
                i = distance;
              }

              cursor += direction;
              continue;
            }
            case 1: {
              //( 1, 1 ) case
              if( Corner* corner = getCorner( cursor ) ) {
                corner->diagonal = segment.faces[ i ];
              } else {
                // Advance to termination event
                i = distance;
              }

              cursor += direction;
              continue;
            }
          }
        }
      }
    }

    
  }

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

      // Insert line segment into cornermap
      insertCornerMapSegment( segment );
    }

    fixCorners();

    // TODO: Generate geometry from cornermap

    return nullptr;
  }

}
