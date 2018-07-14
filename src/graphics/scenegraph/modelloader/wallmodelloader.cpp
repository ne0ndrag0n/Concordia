#include "graphics/scenegraph/modelloader/wallmodelloader.hpp"
#include "graphics/scenegraph/mesh/indexedmeshgenerator.hpp"
#include "graphics/scenegraph/mesh/texturedvertex.hpp"
#include "graphics/scenegraph/model.hpp"
#include "models/walljoint.hpp"
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

  std::shared_ptr< Model > WallModelLoader::get() {
    std::shared_ptr< Model > finalResult = Model::create( "__wallrig", {} );
    std::shared_ptr< Shader > shader = std::make_shared< Shader >( "system/shaders/infr/vertex.glsl", "system/shaders/infr/fragment.glsl" );

    float baseElevation = 0.0f;
    for( const Models::Infrastructure::FloorLevel& floorLevel : levels ) {
      //Mesh::IndexedMeshGenerator< Mesh::TexturedVertex > generator;
      std::vector< std::vector< Models::WallJoint > > jointMap = getJointMap( floorLevel.dimensions, floorLevel.corners );

    }

    return finalResult;
  }

}
