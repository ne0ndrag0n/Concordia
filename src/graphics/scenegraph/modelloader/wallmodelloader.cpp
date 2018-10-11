#include "graphics/scenegraph/modelloader/wallmodelloader.hpp"
#include "graphics/scenegraph/model.hpp"
#include "graphics/texture.hpp"
#include "graphics/shader.hpp"

namespace BlueBear::Graphics::SceneGraph::ModelLoader {

  WallModelLoader::WallModelLoader( const std::vector< Models::Infrastructure::FloorLevel >& levels ) : levels( levels ) {}

  std::shared_ptr< Texture > WallModelLoader::getTexture( const std::string& key, const sf::Image& imageToLoad ) {
    auto pair = textures.find( key );

    if( pair != textures.end() ) {
      return pair->second;
    } else {
      // Add item to texture cache
      return textures[ key ] = std::make_shared< Texture >( imageToLoad );
    }
  }

  std::optional< Models::WallpaperRegion > WallModelLoader::getWallpaperRegion( int x, int y, const WallModelLoader::RegionModelGrid& grid ) {
    if( x < 0 || y < 0 ) {
      return {};
    }

    if( x > grid[ 0 ].size() || y > grid.size() ) {
      return {};
    }

    return grid[ y ][ x ];
  }


  WallModelLoader::CubeSegmentGrid WallModelLoader::regionsToSegments( const WallModelLoader::RegionModelGrid& wallpaperGrid ) {
    CubeSegmentGrid result;

    result.resize( wallpaperGrid.size() );
    for( std::vector< Tools::SegmentedCube< Mesh::TexturedVertex > >& array : result ) {
      array.resize( wallpaperGrid[ 0 ].size() );
    }

    for( int y = 0; y != wallpaperGrid.size(); y++ ) {
      // No irregular regions!
      for( int x = 0; x != wallpaperGrid[ 0 ].size(); x++ ) {
        // TODO:
        // * Check to see if a corner needs to be created (x - 1 and y + 1)
        // * Check to see if a special "R" piece needs to be created
        // * Check other corners to see what texture replacements are necessary
      }
    }

    return result;
  }

  std::shared_ptr< Model > WallModelLoader::generateWallsForStory( const WallModelLoader::CubeSegmentGrid& segmentGrid ) {
    std::shared_ptr< Model > result = Model::create( "__walllevel", {} );

    return result;
  }

  std::shared_ptr< Model > WallModelLoader::get() {
    std::shared_ptr< Model > result = Model::create( "__wallrig", {} );
    rootShader = std::make_shared< Shader >( "system/shaders/infr/vertex.glsl", "system/shaders/infr/fragment.glsl" );

    for( const Models::Infrastructure::FloorLevel& floorLevel : levels ) {
      std::shared_ptr< Model > level = generateWallsForStory( regionsToSegments( floorLevel.wallpapers ) );
    }

    return result;
  }

}
