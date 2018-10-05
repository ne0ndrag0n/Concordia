#include "graphics/scenegraph/modelloader/wallmodelloader.hpp"
#include "graphics/scenegraph/tools/segmentedcube.hpp"
#include "graphics/scenegraph/mesh/texturedvertex.hpp"
#include "graphics/scenegraph/model.hpp"
#include "graphics/shader.hpp"
#include "models/wallpaperregion.hpp"

namespace BlueBear::Graphics::SceneGraph::ModelLoader {

  static std::shared_ptr< Shader > shader = nullptr;

  WallModelLoader::WallModelLoader( const std::vector< Models::Infrastructure::FloorLevel >& levels ) : levels( levels ) {}

  std::vector< std::vector< Tools::SegmentedCube< Mesh::TexturedVertex > > > regionsToSegments( const std::vector< std::vector< Models::WallpaperRegion > >& wallpaperGrid ) {
    std::vector< std::vector< Tools::SegmentedCube< Mesh::TexturedVertex > > > result;

    for( int y = 0; y != wallpaperGrid.size(); y++ ) {
      // No irregular regions!
      for( int x = 0; x != wallpaperGrid[ 0 ].size(); x++ ) {
        
      }
    }

    return result;
  }

  std::shared_ptr< Model > generateWallsForStory( const std::vector< std::vector< Tools::SegmentedCube< Mesh::TexturedVertex > > >& segmentGrid ) {
    std::shared_ptr< Model > result = Model::create( "__walllevel", {} );

    return result;
  }

  std::shared_ptr< Model > WallModelLoader::get() {
    std::shared_ptr< Model > result = Model::create( "__wallrig", {} );
    shader = std::make_shared< Shader >( "system/shaders/infr/vertex.glsl", "system/shaders/infr/fragment.glsl" );

    for( const Models::Infrastructure::FloorLevel& floorLevel : levels ) {
      std::shared_ptr< Model > level = generateWallsForStory( regionsToSegments( floorLevel.wallpapers ) );
    }

    return result;
  }

}
