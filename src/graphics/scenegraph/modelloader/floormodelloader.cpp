#include "graphics/scenegraph/modelloader/floormodelloader.hpp"
#include "graphics/scenegraph/model.hpp"
#include <set>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      namespace ModelLoader {

        FloorModelLoader::FloorModelLoader( const std::vector< Models::Infrastructure::FloorLevel >& levels ) :
          levels( levels ) {}

        void FloorModelLoader::setCurrentLevel( unsigned int level ) {
          currentLevel = level;
        }

        sf::Image FloorModelLoader::generateRootTexture( const glm::uvec2& dimensions ) {
          sf::Image image;

          image.create( dimensions.x, dimensions.y );

          return image;
        }

        std::shared_ptr< Model > FloorModelLoader::get() {
          const Models::Infrastructure::FloorLevel& floorLevel = levels.at( currentLevel );
          std::set< glm::vec3 > vertices;

          // Create parent texture

          for( int y = 0; y != floorLevel.dimensions.y - 1; y++ ) {
            for( int x = 0; x != floorLevel.dimensions.x - 1; x++ ) {
              if( floorLevel.tiles[ y ][ x ] ) {
                // Floor tile is present, add this to the mesh
                // Floor images should be 48x48 images
              } else {
                // Floor tile is not present
              }
            }
          }
        }

      }
    }
  }
}
