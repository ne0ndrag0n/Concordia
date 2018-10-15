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

    if( x >= grid[ 0 ].size() || y >= grid.size() ) {
      return {};
    }

    return grid[ y ][ x ];
  }

  std::array< Mesh::TexturedVertex, 6 > WallModelLoader::getPlane( const glm::vec3& origin, const glm::vec3& horizontalDirection, const glm::vec3& verticalDirection ) {
    std::array< Mesh::TexturedVertex, 6 > plane;

    plane[ 0 ] = { origin,                                           { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } };
    plane[ 1 ] = { origin + horizontalDirection,                     { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } };
    plane[ 2 ] = { origin + verticalDirection,                       { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } };

    plane[ 3 ] = { origin + horizontalDirection,                     { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } };
    plane[ 4 ] = { origin + horizontalDirection + verticalDirection, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } };
    plane[ 5 ] = { origin + verticalDirection,                       { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } };

    return plane;
  }

  Tools::SegmentedCube< Mesh::TexturedVertex > WallModelLoader::getSegmentedCubeRegular( const glm::vec3& origin, const glm::vec3& dimensions ) {
    Tools::SegmentedCube< Mesh::TexturedVertex > result;

    result.south = { getPlane( origin,                                                  { dimensions.x, 0.0f, 0.0f  }, { 0.0f, 0.0f, dimensions.z  } ), nullptr };
    result.east  = { getPlane( origin + glm::vec3{ dimensions.x, 0.0f, 0.0f },          { 0.0f, dimensions.y, 0.0f  }, { 0.0f, 0.0f, dimensions.z  } ), nullptr };
    result.north = { getPlane( origin + glm::vec3{ dimensions.x, -dimensions.y, 0.0f }, { -dimensions.x, 0.0f, 0.0f }, { 0.0f, 0.0f, dimensions.z  } ), nullptr };
    result.west  = { getPlane( origin + glm::vec3{ 0.0f, -dimensions.y, 0.0f },         { 0.0f, -dimensions.y, 0.0f }, { 0.0f, 0.0f, dimensions.z  } ), nullptr };
    result.top   = { getPlane( origin + glm::vec3{ 0.0f, 0.0f, dimensions.z },          { dimensions.x, 0.0f, 0.0f  }, { 0.0f, -dimensions.y, 0.0f } ), nullptr };

    return result;
  }

  Tools::SegmentedCube< Mesh::TexturedVertex > WallModelLoader::getCornerCube( const glm::vec3& origin, const glm::vec3& dimensions ) {
    Tools::SegmentedCube< Mesh::TexturedVertex > result;

    result.south = { getPlane( origin,                                                  { dimensions.x, 0.0f, 0.0f  }, { 0.0f, 0.0f, dimensions.z  } ), nullptr };
    result.east  = { getPlane( origin + glm::vec3{ dimensions.x, 0.0f, 0.0f },          { 0.0f, dimensions.y, 0.0f  }, { 0.0f, 0.0f, dimensions.z  } ), nullptr };
    result.top   = { getPlane( origin + glm::vec3{ 0.0f, 0.0f, dimensions.z },          { dimensions.x, 0.0f, 0.0f  }, { 0.0f, -dimensions.y, 0.0f } ), nullptr };

    return result;
  }

  bool WallModelLoader::isCornerFillRequired( int x, int y, const WallModelLoader::RegionModelGrid& wallpaperGrid ) {
    std::optional< Models::WallpaperRegion > left = getWallpaperRegion( x - 1, y, wallpaperGrid );
    std::optional< Models::WallpaperRegion > top = getWallpaperRegion( x, y - 1, wallpaperGrid );

    return left && top && left->isX() && top->isY();
  }

  /**
   * Steal textures and adjust surrounding texture coordinates
   *
   * This function is not to be called unless the conditions have been verified
   */
  void WallModelLoader::joinCorner( int x, int y, WallModelLoader::CubeSegmentGrid& cubeSegmentGrid ) {
    Tools::SegmentedCube< Mesh::TexturedVertex >& subject = *cubeSegmentGrid[ y ][ x ];
    Tools::SegmentedCube< Mesh::TexturedVertex >& left = *cubeSegmentGrid[ y ][ x - 1 ];
    Tools::SegmentedCube< Mesh::TexturedVertex >& top = *cubeSegmentGrid[ y - 1 ][ x ];

    // -----------------------------------------------------------------------------------------------

    // Steal south texture
    subject.south->texture = left.south->texture;

    // Left.south needs coordinates adjusted
    left.south->vertexData[ 1 ].textureCoordinates = left.south->vertexData[ 3 ].textureCoordinates = { 0.91f, 0.0f };
    left.south->vertexData[ 4 ].textureCoordinates = { 0.91f, 1.0f };

    // Set subject coordinates
    subject.south->vertexData[ 0 ].textureCoordinates = { 0.91f, 0.0f };
    subject.south->vertexData[ 2 ].textureCoordinates = subject.south->vertexData[ 5 ].textureCoordinates = { 0.91f, 1.0f };

    // -----------------------------------------------------------------------------------------------

    // Steal east texture
    subject.east->texture = top.east->texture;

    // top.east needs coordinates adjusted
    top.east->vertexData[ 0 ].textureCoordinates = { 0.09f, 0.0f };
    top.east->vertexData[ 2 ].textureCoordinates = top.east->vertexData[ 5 ].textureCoordinates = { 0.09f, 1.0f };

    // Set subject coordinates
    subject.east->vertexData[ 1 ].textureCoordinates = subject.east->vertexData[ 3 ].textureCoordinates = { 0.09f, 0.0f };
    subject.east->vertexData[ 4 ].textureCoordinates = { 0.09f, 1.0f };
  }

  WallModelLoader::CubeSegmentGrid WallModelLoader::regionsToSegments( const WallModelLoader::RegionModelGrid& wallpaperGrid ) {
    CubeSegmentGrid result;

    result.resize( wallpaperGrid.size() );
    for( auto& array : result ) {
      array.resize( wallpaperGrid[ 0 ].size() );
    }

    glm::vec2 origin = { -( wallpaperGrid[ 0 ].size() * 0.5 ), wallpaperGrid.size() * 0.5 };
    for( int y = 0; y != wallpaperGrid.size(); y++ ) {
      // No irregular regions!
      for( int x = 0; x != wallpaperGrid[ 0 ].size(); x++ ) {
        const Models::WallpaperRegion& wallpaperRegion = wallpaperGrid[ y ][ x ];

        if( wallpaperRegion.isEmpty() && isCornerFillRequired( x, y, wallpaperGrid ) ) {
          // Corner needs to be created
          result[ y ][ x ] = getCornerCube( { -0.5f, 0.9f, 0.0f }, { 0.1f, 0.1f, 4.0f } );

          // Texture coordinates of neighbours and self need to be corrected
          // Textures of neighbours need to be stolen
          joinCorner( x, y, result );
        }


        if( wallpaperRegion.isCorner() ) {
          // Check to see if a special "R" piece needs to be created
        }

        // Check other corners to see what texture replacements are necessary

        // Generate ordinary piece
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

    float base = 0.0f;
    for( const Models::Infrastructure::FloorLevel& floorLevel : levels ) {
      std::shared_ptr< Model > level = generateWallsForStory( regionsToSegments( floorLevel.wallpapers ) );
      base += 4.0f;
    }

    return result;
  }

}
