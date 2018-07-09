#include "graphics/scenegraph/modelloader/wallmodelloader.hpp"
#include "graphics/scenegraph/mesh/indexedmeshgenerator.hpp"
#include "graphics/scenegraph/mesh/texturedvertex.hpp"
#include "graphics/scenegraph/model.hpp"
#include "graphics/shader.hpp"

namespace BlueBear::Graphics::SceneGraph::ModelLoader {

  WallModelLoader::WallModelLoader( const std::vector< Models::Infrastructure::FloorLevel >& levels ) : levels( levels ) {}

  sf::Image WallModelLoader::generateTexture( const Models::Infrastructure::FloorLevel& currentLevel ) {
    // TODO
    return sf::Image();
  }

  std::shared_ptr< Model > WallModelLoader::get() {
    std::shared_ptr< Model > finalResult = Model::create( "__wallrig", {} );
    std::shared_ptr< Shader > shader = std::make_shared< Shader >( "system/shaders/infr/vertex.glsl", "system/shaders/infr/fragment.glsl" );

    float baseElevation = 0.0f;
    for( const Models::Infrastructure::FloorLevel& floorLevel : levels ) {
      Mesh::IndexedMeshGenerator< Mesh::TexturedVertex > generator;
      
    }

    return finalResult;
  }

}
