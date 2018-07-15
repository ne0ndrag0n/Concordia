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

  WallModelLoader::Face WallModelLoader::generateFace( const glm::vec3& lowerLeftCorner, const glm::vec2& dimensions ) {
    Face results;

    results[ 0 ] = { { lowerLeftCorner.x,                lowerLeftCorner.y,                 lowerLeftCorner.z                }, { 0.0, 0.0, 0.0 }, { 0.0, 0.0 } };
    results[ 1 ] = { { lowerLeftCorner.x + dimensions.x, lowerLeftCorner.y + dimensions.y,  lowerLeftCorner.z                }, { 0.0, 0.0, 0.0 }, { 0.0, 0.0 } };
    results[ 2 ] = { { lowerLeftCorner.x,                lowerLeftCorner.y,                 lowerLeftCorner.z + 4.0          }, { 0.0, 0.0, 0.0 }, { 0.0, 0.0 } };

    results[ 3 ] = { { lowerLeftCorner.x + dimensions.x, lowerLeftCorner.y + dimensions.y,  lowerLeftCorner.z                }, { 0.0, 0.0, 0.0 }, { 0.0, 0.0 } };
    results[ 4 ] = { { lowerLeftCorner.x + dimensions.x, lowerLeftCorner.y + dimensions.y,  lowerLeftCorner.z + 4.0          }, { 0.0, 0.0, 0.0 }, { 0.0, 0.0 } };
    results[ 5 ] = { { lowerLeftCorner.x,                lowerLeftCorner.y,                 lowerLeftCorner.z + 4.0          }, { 0.0, 0.0, 0.0 }, { 0.0, 0.0 } };

    return results;
  }

  void WallModelLoader::addTrianglesForJoint(
    Mesh::IndexedMeshGenerator< Mesh::TexturedVertex >& generator,
    const Models::WallJoint& wallJoint,
    const glm::vec3& origin
  ) {
    if( wallJoint.isSingleAxis() ) {
      if( wallJoint.east ) {
        Face southFace = generateFace( { origin.x, origin.y - 0.05f, origin.z }, { 0.5, 0.0 } );
        generator.addTriangle( southFace[ 0 ], southFace[ 1 ], southFace[ 2 ] );
        generator.addTriangle( southFace[ 3 ], southFace[ 4 ], southFace[ 5 ] );

        Face eastFace = generateFace( { origin.x + 0.5, origin.y - 0.05f, origin.z }, { 0.0, 0.1 } );
        generator.addTriangle( eastFace[ 0 ], eastFace[ 1 ], eastFace[ 2 ] );
        generator.addTriangle( eastFace[ 3 ], eastFace[ 4 ], eastFace[ 5 ] );

        Face northFace = generateFace( { origin.x + 0.5, origin.y + 0.05f, origin.z }, { -0.5, 0.0 } );
        generator.addTriangle( northFace[ 0 ], northFace[ 1 ], northFace[ 2 ] );
        generator.addTriangle( northFace[ 3 ], northFace[ 4 ], northFace[ 5 ] );

        Face westFace = generateFace( { origin.x, origin.y + 0.05f, origin.z }, { 0.0, -0.1 } );
        generator.addTriangle( westFace[ 0 ], westFace[ 1 ], westFace[ 2 ] );
        generator.addTriangle( westFace[ 3 ], westFace[ 4 ], westFace[ 5 ] );
      }

      if( wallJoint.west ) {
        Face southFace = generateFace( { origin.x - 0.5f, origin.y - 0.05f, origin.z }, { 0.5, 0.0 } );
        generator.addTriangle( southFace[ 0 ], southFace[ 1 ], southFace[ 2 ] );
        generator.addTriangle( southFace[ 3 ], southFace[ 4 ], southFace[ 5 ] );

        Face eastFace = generateFace( { origin.x, origin.y - 0.05f, origin.z }, { 0.0, 0.1 } );
        generator.addTriangle( eastFace[ 0 ], eastFace[ 1 ], eastFace[ 2 ] );
        generator.addTriangle( eastFace[ 3 ], eastFace[ 4 ], eastFace[ 5 ] );

        Face northFace = generateFace( { origin.x, origin.y + 0.05f, origin.z }, { -0.5, 0.0 } );
        generator.addTriangle( northFace[ 0 ], northFace[ 1 ], northFace[ 2 ] );
        generator.addTriangle( northFace[ 3 ], northFace[ 4 ], northFace[ 5 ] );

        Face westFace = generateFace( { origin.x - 0.5f, origin.y + 0.05f, origin.z }, { 0.0, -0.1 } );
        generator.addTriangle( westFace[ 0 ], westFace[ 1 ], westFace[ 2 ] );
        generator.addTriangle( westFace[ 3 ], westFace[ 4 ], westFace[ 5 ] );
      }
    } else if( wallJoint.isElbow() ) {
      Log::getInstance().error( "WallModelLoader::addTrianglesForJoint", "Elbow not yet implemented." );
    } else if( wallJoint.isCross() ) {
      Log::getInstance().error( "WallModelLoader::addTrianglesForJoint", "Cross not yet implemented." );
    } else if( wallJoint.isFull() ){
      Log::getInstance().error( "WallModelLoader::addTrianglesForJoint", "Full not yet implemented." );
    } else {
      // There is nothing to generate
    }
  }

  std::shared_ptr< Model > WallModelLoader::get() {
    std::shared_ptr< Model > finalResult = Model::create( "__wallrig", {} );
    std::shared_ptr< Shader > shader = std::make_shared< Shader >( "system/shaders/infr/vertex.glsl", "system/shaders/infr/fragment.glsl" );

    float baseElevation = 0.0f;
    for( const Models::Infrastructure::FloorLevel& floorLevel : levels ) {
      std::vector< std::vector< Models::WallJoint > > jointMap = getJointMap( floorLevel.dimensions, floorLevel.corners );

      Mesh::IndexedMeshGenerator< Mesh::TexturedVertex > generator;
      const glm::uvec2 boundaries = floorLevel.dimensions + glm::uvec2{ 1, 1 };
      const glm::vec2 origin = {
        ( ( float ) floorLevel.dimensions.x ) * -0.5,
        ( ( float ) floorLevel.dimensions.y ) * 0.5
      };
      for( unsigned int y = 0; y != boundaries.y; y++ ) {
        for( unsigned int x = 0; x != boundaries.x; x++ ) {
          addTrianglesForJoint( generator, jointMap[ y ][ x ], { origin.x + x, origin.y + y, baseElevation } );
        }
      }
    }

    return finalResult;
  }

}
