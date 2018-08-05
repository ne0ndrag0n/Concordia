#include "graphics/scenegraph/modelloader/wallmodelloader.hpp"
#include "graphics/scenegraph/model.hpp"
#include "graphics/shader.hpp"
#include "log.hpp"

namespace BlueBear::Graphics::SceneGraph::ModelLoader {

  WallModelLoader::WallModelLoader( const std::vector< Models::Infrastructure::FloorLevel >& levels ) : levels( levels ) {}

  sf::Image WallModelLoader::generateTexture( const Models::Infrastructure::FloorLevel& currentLevel ) {
    // TODO
    return sf::Image();
  }

  std::vector< std::vector< Models::WallJoint > > WallModelLoader::getJointMap( const glm::uvec2& dimensions, const std::vector< std::pair< glm::uvec2, glm::uvec2 > >& corners ) {
    std::vector< std::vector< Models::WallJoint > > result;

    result.resize( dimensions.y );
    for( auto& array : result ) {
      array.resize( dimensions.x );
    }

    for( const auto& corner : corners ) {
      if( corner.first.x == corner.second.x ) {
        // Progression occurs along the y-axis
        unsigned int start = std::min( corner.first.y, corner.second.y );
        unsigned int end = std::max( corner.first.y, corner.second.y );

        for( unsigned int i = start; i <= end; i++ ) {
          if( i == start ) {
            result[ i ][ corner.first.x ].south = true;
          } else if( i == end ) {
            result[ i ][ corner.first.x ].north = true;
          } else {
            result[ i ][ corner.first.x ].south =
            result[ i ][ corner.first.x ].north =
            true;
          }
        }
      } else if( corner.first.y == corner.second.y ) {
        // Progression occurs along the x-axis
        unsigned int start = std::min( corner.first.x, corner.second.x );
        unsigned int end = std::max( corner.first.x, corner.second.x );

        for( unsigned int i = start; i <= end; i++ ) {
          if( i == start ) {
            result[ corner.first.y ][ i ].east = true;
          } else if( i == end ) {
            result[ corner.first.y ][ i ].west = true;
          } else {
            result[ corner.first.y ][ i ].east =
            result[ corner.first.y ][ i ].west =
            true;
          }
        }
      } else {
        Log::getInstance().error( "WallModelLoader::getJointMap", "Corner map appears invalid" );
      }
    }

    return result;
  }

  WallModelLoader::FaceSet WallModelLoader::getSingleAxisFaceSet( const Models::WallJoint& joint, const glm::vec2& position, WallModelLoader::WallpaperNeighborhood wallpapers, WallModelLoader::VertexNeighborhood vertices ) {
    if( joint.north ) {
      // TODO
    }
  }

  WallModelLoader::FaceSet WallModelLoader::getFaceSet( const glm::uvec2& position, const Models::Infrastructure::FloorLevel& floorLevel, const JointMap& jointMap ) {
    const Models::WallJoint& joint = jointMap[ position.y ][ position.x ];
    const glm::vec2 origin = { ( ( float ) floorLevel.dimensions.x ) * -0.5, ( ( float ) floorLevel.dimensions.y ) * 0.5 };

    if( joint.isSingleAxis() ) {
      return getSingleAxisFaceSet(
        joint,
        glm::vec2{ origin.x + position.x, -( origin.y + position.y ) },
        getWallpaperNeighborhood( position, floorLevel ),
        getVertexNeighborhood( position, floorLevel )
      );
    }

    // TODO
  }

  static inline bool isValidCoordinate( const glm::ivec2& position, const glm::uvec2& boundary ) {
    return ( position.x >= 0 && position.y >= 0 ) &&
      ( position.x < boundary.x && position.y < boundary.y );
  }

  WallModelLoader::WallpaperNeighborhood WallModelLoader::getWallpaperNeighborhood( glm::ivec2 position, const Models::Infrastructure::FloorLevel& floorLevel ) {
    WallModelLoader::WallpaperNeighborhood neighborhood;

    // Lower right corner
    if( isValidCoordinate( position, floorLevel.dimensions ) ) {
      neighborhood[ 0 ] = floorLevel.wallpapers[ position.y ][ position.x ];
    }
    // Lower left corner
    if( isValidCoordinate( position - glm::ivec2{ 1, 0 }, floorLevel.dimensions ) ) {
      neighborhood[ 1 ] = floorLevel.wallpapers[ position.y ][ position.x - 1 ];
    }
    // Upper left corner
    if( isValidCoordinate( position - glm::ivec2{ 1, 1 }, floorLevel.dimensions ) ) {
      neighborhood[ 2 ] = floorLevel.wallpapers[ position.y - 1 ][ position.x - 1 ];
    }
    // Upper right corner
    if( isValidCoordinate( position - glm::ivec2{ 0, 1 }, floorLevel.dimensions ) ) {
      neighborhood[ 3 ] = floorLevel.wallpapers[ position.y - 1 ][ position.x ];
    }

    return neighborhood;
  }

  WallModelLoader::VertexNeighborhood WallModelLoader::getVertexNeighborhood( glm::ivec2 position, const Models::Infrastructure::FloorLevel& floorLevel ) {
    WallModelLoader::VertexNeighborhood neighborhood;

    glm::uvec2 dimensions = floorLevel.dimensions + glm::uvec2{ 1, 1 };
    // Lower right corner
    if( isValidCoordinate( position, dimensions ) ) {
      neighborhood[ 0 ] = floorLevel.vertices[ position.y ][ position.x ];
    }
    // Lower left corner
    if( isValidCoordinate( position - glm::ivec2{ 1, 0 }, dimensions ) ) {
      neighborhood[ 1 ] = floorLevel.vertices[ position.y ][ position.x - 1 ];
    }
    // Upper left corner
    if( isValidCoordinate( position - glm::ivec2{ 1, 1 }, dimensions ) ) {
      neighborhood[ 2 ] = floorLevel.vertices[ position.y - 1 ][ position.x - 1 ];
    }
    // Upper right corner
    if( isValidCoordinate( position - glm::ivec2{ 0, 1 }, dimensions ) ) {
      neighborhood[ 3 ] = floorLevel.vertices[ position.y - 1 ][ position.x ];
    }

    return neighborhood;
  }

  std::shared_ptr< Model > WallModelLoader::get() {
    std::shared_ptr< Model > finalResult = Model::create( "__wallrig", {} );
    std::shared_ptr< Shader > shader = std::make_shared< Shader >( "system/shaders/infr/vertex.glsl", "system/shaders/infr/fragment.glsl" );

    float baseElevation = 0.0f;
    for( const Models::Infrastructure::FloorLevel& floorLevel : levels ) {
      JointMap jointMap = getJointMap( floorLevel.dimensions, floorLevel.corners );

      Mesh::IndexedMeshGenerator< Mesh::TexturedVertex > generator;
      const glm::uvec2 boundaries = floorLevel.dimensions + glm::uvec2{ 1, 1 };
      for( unsigned int y = 0; y != boundaries.y; y++ ) {
        for( unsigned int x = 0; x != boundaries.x; x++ ) {
          FaceSet faceSet = getFaceSet( { x, y }, floorLevel, jointMap );
          // TODO
        }
      }
    }

    return finalResult;
  }

}
