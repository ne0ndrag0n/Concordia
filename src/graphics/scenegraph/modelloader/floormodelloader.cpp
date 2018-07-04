#include "graphics/scenegraph/modelloader/floormodelloader.hpp"
#include "graphics/scenegraph/mesh/indexedmeshgenerator.hpp"
#include "graphics/scenegraph/transform.hpp"
#include "graphics/scenegraph/drawable.hpp"
#include "graphics/scenegraph/model.hpp"
#include "graphics/scenegraph/material.hpp"
#include "graphics/texture.hpp"
#include "graphics/shader.hpp"
#include "configmanager.hpp"
#include "log.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      namespace ModelLoader {

        FloorModelLoader::FloorModelLoader( const std::vector< Models::Infrastructure::FloorLevel >& levels ) :
          levels( levels ) {}

        sf::Image FloorModelLoader::generateTexture( const Models::Infrastructure::FloorLevel& currentLevel ) {
          sf::Image result;

          int side = ConfigManager::getInstance().getIntValue( "floor_texture_size" );
          result.create( currentLevel.dimensions.x * side, currentLevel.dimensions.y * side );

          return result;
        }

        std::shared_ptr< Model > FloorModelLoader::get() {
          int side = ConfigManager::getInstance().getIntValue( "floor_texture_size" );
          std::shared_ptr< Model > finalResult = Model::create( "__floorrig", {} );
          std::shared_ptr< Shader > shader = std::make_shared< Shader >( "system/shaders/infr/vertex.glsl", "system/shaders/infr/fragment.glsl" );

          float baseElevation = 0.0f;
          for( const Models::Infrastructure::FloorLevel& floorLevel : levels ) {
            Mesh::IndexedMeshGenerator< Mesh::TexturedVertex > generator;
            sf::Image meshTexture = generateTexture( floorLevel );

            glm::vec2 dims = floorLevel.dimensions;
            glm::vec2 origin = { -( dims.x * 0.5 ), dims.y * 0.5 };
            glm::vec2 last = { floorLevel.dimensions.x, floorLevel.dimensions.y };
            for( int y = 0; y != floorLevel.dimensions.y; y++ ) {
              for( int x = 0; x != floorLevel.dimensions.x; x++ ) {
                glm::vec2 current = { x, y };

                if( floorLevel.tiles[ y ][ x ] ) {
                  meshTexture.copy( *floorLevel.tiles[ y ][ x ]->surface, x * side, y * side );

                  glm::vec2 base = { origin.x + x, origin.y - y };
                  glm::vec2 baseRight = { base.x + 1.0, base.y };
                  glm::vec2 baseDiagonal = { base.x + 1.0, base.y - 1.0 };
                  glm::vec2 baseDown = { base.x, base.y - 1.0 };

                  // Winding direction must be counterclockwise
                  generator.addTriangle(
                    { { baseDown.x, baseDown.y, baseElevation + floorLevel.vertices[ y + 1 ][ x ] },                { 0.0, 0.0, 0.0 }, { current.x / last.x, 1.0 - ( ( current.y + 1.0 ) / last.y ) } },
                    { { baseRight.x, baseRight.y, baseElevation + floorLevel.vertices[ y ][ x + 1 ] },              { 0.0, 0.0, 0.0 }, { ( current.x + 1.0 ) / last.x, 1.0 - ( current.y / last.y ) } },
                    { { base.x, base.y, baseElevation + floorLevel.vertices[ y ][ x ] },                            { 0.0, 0.0, 0.0 }, { current.x / last.x, 1.0 - ( current.y / last.y ) } }
                  );
                  generator.addTriangle(
                    { { baseDown.x, baseDown.y, baseElevation + floorLevel.vertices[ y + 1 ][ x ] },                { 0.0, 0.0, 0.0 }, { current.x / last.x, 1.0 - ( ( current.y + 1.0 ) / last.y ) } },
                    { { baseDiagonal.x, baseDiagonal.y, baseElevation + floorLevel.vertices[ y + 1 ][ x + 1 ] },    { 0.0, 0.0, 0.0 }, { ( current.x + 1.0 ) / last.x, 1.0 - ( ( current.y + 1.0 ) / last.y ) } },
                    { { baseRight.x, baseRight.y, baseElevation + floorLevel.vertices[ y ][ x + 1 ] },              { 0.0, 0.0, 0.0 }, { ( current.x + 1.0 ) / last.x, 1.0 - ( current.y / last.y ) } }
                  );
                }
              }
            }

            meshTexture.flipVertically();
            generator.generateNormals();

            std::shared_ptr< Model > result = Model::create( "__floorlevel", { {
              generator.generateMesh(),
              shader,
              std::make_shared< Material >( std::vector< std::shared_ptr< Texture > >{ std::make_shared< Texture >( meshTexture ) }, std::vector< std::shared_ptr< Texture > >{}, 0.0f, 1.0f )
            } } );

            finalResult->addChild( result );

            baseElevation += 10.0f;
          }

          return finalResult;
        }

      }
    }
  }
}
