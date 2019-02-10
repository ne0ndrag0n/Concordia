#include "graphics/scenegraph/modelloader/wallmodelloader.hpp"
#include "graphics/scenegraph/model.hpp"
#include "graphics/scenegraph/material.hpp"
#include "graphics/vector/renderer.hpp"
#include "graphics/texture.hpp"
#include "graphics/shader.hpp"
#include "exceptions/nullpointerexception.hpp"
#include "tools/utility.hpp"
#include <glm/gtx/rotate_vector.hpp>

namespace BlueBear::Graphics::SceneGraph::ModelLoader {

  WallModelLoader::WallModelLoader( const std::vector< Models::Infrastructure::FloorLevel >& floorLevels, Vector::Renderer& renderer )
    : floorLevels( floorLevels ), shader( std::make_shared< Shader >( "system/shaders/infr/vertex.glsl", "system/shaders/infr/fragment.glsl" ) ) {
      initTopTexture( renderer );
    }

  void WallModelLoader::initTopTexture( Vector::Renderer& renderer ) {
    std::shared_ptr< unsigned char[] > rawBitmap = renderer.generateBitmap(
      { 10, 10 },
      []( Vector::Renderer& r ) {
        r.drawRect( { 0, 0, 10, 10 }, { 143, 89, 2, 255 } );
      }
    );

    std::shared_ptr< sf::Image > sfmlImage = std::make_shared< sf::Image >();
    sfmlImage->create( 10, 10, rawBitmap.get() );

    atlas.addTexture( "__top_side", sfmlImage );
  }

  void WallModelLoader::initCornerMap() {
    cornerMap.clear();

    cornerMap.resize( dimensions.y );
    for( auto& xLevel : cornerMap ) {
      xLevel.resize( dimensions.x );
    }
  }

  bool WallModelLoader::adjustTopLeft( const glm::ivec2& index ) {
    Corner* corner = getCorner( index );
    if( !corner ) {
      return false;
    }

    bool isElbow = corner->horizontal.model && corner->vertical.model;

    Corner* upper = getCorner( index + glm::ivec2{ 0, -1 } );
    bool upperClear = !upper || !upper->vertical.model;

    Corner* left = getCorner( index + glm::ivec2{ -1, 0 } );
    bool leftClear = !left || !left->horizontal.model;

    return isElbow && upperClear && leftClear;
  }

  bool WallModelLoader::adjustTopRight( const glm::ivec2& index ) {
    Corner* corner = getCorner( index );
    if( !corner ) {
      return false;
    }

    bool isSingleVertical = corner->vertical.model && !corner->horizontal.model;

    Corner* left = getCorner( index + glm::ivec2{ -1, 0 } );
    bool leftHorizontal = left && left->horizontal.model;

    Corner* upper = getCorner( index + glm::ivec2{ 0, -1 } );
    bool upperClear = !upper || !upper->vertical.model;

    return isSingleVertical && leftHorizontal && upperClear;
  }

  bool WallModelLoader::adjustBottomLeft( const glm::ivec2& index ) {
    Corner* corner = getCorner( index );
    if( !corner ) {
      return false;
    }

    bool isSingleHorizontal = !corner->vertical.model && corner->horizontal.model;

    Corner* left = getCorner( index + glm::ivec2{ -1, 0 } );
    bool leftClear = !left || !left->horizontal.model;

    Corner* upper = getCorner( index + glm::ivec2{ 0, -1 } );
    bool upperVertical = upper && upper->vertical.model;

    return isSingleHorizontal && leftClear && upperVertical;
  }

  bool WallModelLoader::adjustBottomRight( const glm::ivec2& index ) {
    Corner* corner = getCorner( index );
    if( !corner ) {
      return false;
    }

    bool isSingleHorizontal = !corner->vertical.model && corner->horizontal.model;

    Corner* right = getCorner( index + glm::ivec2{ 1, 0 } );
    bool rightClear = !right || ( !right->horizontal.model && !right->vertical.model );

    Corner* upperRight = getCorner( index + glm::ivec2{ 1, 1 } );
    bool upperRightVertical = upperRight && upperRight->vertical.model;

    return isSingleHorizontal && rightClear && upperRightVertical;
  }

  glm::vec3 WallModelLoader::indexToLocation( const glm::ivec2& position ) {
    // TODO: Elevation per vertex
    return { -( dimensions.x * 0.5f ) + position.x, ( dimensions.y * 0.5f ) + position.y, 0.0f };
  }

  void WallModelLoader::fixCorners( const glm::ivec2& startingIndex ) {
    // TODO: Elevation per vertex

    if( adjustTopLeft( startingIndex ) ) {
      Corner* corner;
      Exceptions::NullPointerException::check( "WallModelLoader::fixCorners", corner = getCorner( startingIndex ) );
      updateStagedMesh( corner->horizontal.stagedMesh, indexToLocation( startingIndex ) + glm::vec3{ 0.0f, 0.05f, 0.0f }, { -0.05f, 0.0f, 0.0f } );
      updateStagedMesh( corner->horizontal.stagedMesh, indexToLocation( startingIndex ) + glm::vec3{ 0.0f, 0.05f, 4.0f }, { -0.05f, 0.0f, 0.0f } );

      updateStagedMesh( corner->vertical.stagedMesh, indexToLocation( startingIndex ) + glm::vec3{ -0.05f, 0.0f, 0.0f }, { 0.0f, 0.05f, 0.0f } );
      updateStagedMesh( corner->vertical.stagedMesh, indexToLocation( startingIndex ) + glm::vec3{ -0.05f, 0.0f, 4.0f }, { 0.0f, 0.05f, 0.0f } );
    }

    if( adjustTopRight( startingIndex ) ) {
      Corner* corner;
      Corner* left;
      Exceptions::NullPointerException::check( "WallModelLoader::fixCorners", corner = getCorner( startingIndex ) );
      Exceptions::NullPointerException::check( "WallModelLoader::fixCorners", left = getCorner( startingIndex + glm::ivec2{ -1, 0 } ) );

      updateStagedMesh( corner->vertical.stagedMesh, indexToLocation( startingIndex ) + glm::vec3{ 0.05f, 0.0f, 0.0f }, { 0.0f, 0.05f, 0.0f } );
      updateStagedMesh( corner->vertical.stagedMesh, indexToLocation( startingIndex ) + glm::vec3{ 0.05f, 0.0f, 4.0f }, { 0.0f, 0.05f, 0.0f } );

      updateStagedMesh( left->horizontal.stagedMesh, indexToLocation( startingIndex + glm::ivec2{ -1, 0 } ) + glm::vec3{ 1.0f, 0.05f, 0.0f }, { 0.05f, 0.0f, 0.0f } );
      updateStagedMesh( left->horizontal.stagedMesh, indexToLocation( startingIndex + glm::ivec2{ -1, 0 } ) + glm::vec3{ 1.0f, 0.05f, 4.0f }, { 0.05f, 0.0f, 0.0f } );
    }

    if( adjustBottomLeft( startingIndex ) ) {
      Corner* corner;
      Corner* top;
      Exceptions::NullPointerException::check( "WallModelLoader::fixCorners", corner = getCorner( startingIndex ) );
      Exceptions::NullPointerException::check( "WallModelLoader::fixCorners", top = getCorner( startingIndex + glm::ivec2{ 0, -1 } ) );

      updateStagedMesh( corner->horizontal.stagedMesh, indexToLocation( startingIndex ) + glm::vec3{ 0.0f, -0.05f, 0.0f }, { -0.05f, 0.0f, 0.0f } );
      updateStagedMesh( corner->horizontal.stagedMesh, indexToLocation( startingIndex ) + glm::vec3{ 0.0f, -0.05f, 4.0f }, { -0.05f, 0.0f, 0.0f } );

      updateStagedMesh( top->vertical.stagedMesh, indexToLocation( startingIndex + glm::ivec2{ 0, -1 } ) + glm::vec3{ -0.05f, -1.0f, 0.0f }, { 0.0f, -0.05f, 0.0f } );
      updateStagedMesh( top->vertical.stagedMesh, indexToLocation( startingIndex + glm::ivec2{ 0, -1 } ) + glm::vec3{ -0.05f, -1.0f, 4.0f }, { 0.0f, -0.05f, 0.0f } );
    }

    if( adjustBottomRight( startingIndex ) ) {
      Corner* corner;
      Corner* upperRight;
      Exceptions::NullPointerException::check( "WallModelLoader::fixCorners", corner = getCorner( startingIndex ) );
      Exceptions::NullPointerException::check( "WallModelLoader::fixCorners", upperRight = getCorner( startingIndex + glm::ivec2{ 1, -1 } ) );

      updateStagedMesh( corner->horizontal.stagedMesh, indexToLocation( startingIndex ) + glm::vec3{ 1.0f, -0.05f, 0.0f }, { 0.05f, 0.0f, 0.0f } );
      updateStagedMesh( corner->horizontal.stagedMesh, indexToLocation( startingIndex ) + glm::vec3{ 1.0f, -0.05f, 4.0f }, { 0.05f, 0.0f, 0.0f } );

      updateStagedMesh( upperRight->vertical.stagedMesh, indexToLocation( startingIndex + glm::ivec2{ 1, -1 } ) + glm::vec3{ 0.05f, -1.0f, 0.0f }, { 0.0f, -0.05f, 0.0f } );
      updateStagedMesh( upperRight->vertical.stagedMesh, indexToLocation( startingIndex + glm::ivec2{ 1, -1 } ) + glm::vec3{ 0.05f, -1.0f, 4.0f }, { 0.0f, -0.05f, 0.0f } );
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

  void WallModelLoader::updateStagedMesh( PlaneGroup& group, const glm::vec3& position, const glm::vec3& addValue ) {
    for( auto& pair : group ) {
      for( auto& vertex : pair.second ) {
        if( Tools::Utility::equalEpsilon( vertex.position, position ) ) {
          vertex.position += addValue;
        }
      }
    }
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
                corner->diagonal.model = segment.faces[ i ];
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
                corner->horizontal.model = segment.faces[ i ];
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
                corner->reverseDiagonal.model = segment.faces[ i ];
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
                corner->vertical.model = segment.faces[ i ];
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
                corner->vertical.model = segment.faces[ i ];
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
                corner->reverseDiagonal.model = segment.faces[ i ];
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
                corner->horizontal.model = segment.faces[ i ];
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
                corner->diagonal.model = segment.faces[ i ];
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

  void WallModelLoader::insertIntoAtlas( const std::vector< Models::Sides >& sides, Utilities::TextureAtlas& atlas ) {
    for( const auto& side : sides ) {
      const auto& [ frontWallpaperId, frontWallpaper ] = side.front;
      const auto& [ backWallpaperId, backWallpaper ] = side.back;

      atlas.addTexture( frontWallpaperId, frontWallpaper.surface );
      atlas.addTexture( backWallpaperId, backWallpaper.surface );
    }
  }

  std::array< Mesh::TexturedVertex, 6 > WallModelLoader::getPlane( const glm::vec3& origin, const glm::vec3& width, const glm::vec3& height, const std::string& wallpaperId ) {
    std::array< Mesh::TexturedVertex, 6 > plane;
    auto textureData = atlas.getTextureData( wallpaperId );

    plane[ 0 ] = { origin,                             { 0.0f, 0.0f, 0.0f }, textureData.lowerCorner };
    plane[ 1 ] = { origin + width,                     { 0.0f, 0.0f, 0.0f }, { textureData.upperCorner.x, textureData.lowerCorner.y } };
    plane[ 2 ] = { origin + height,                    { 0.0f, 0.0f, 0.0f }, { textureData.lowerCorner.x, textureData.upperCorner.y } };

    plane[ 3 ] = { origin + width,                     { 0.0f, 0.0f, 0.0f }, { textureData.upperCorner.x, textureData.lowerCorner.y } };
    plane[ 4 ] = { origin + width + height,            { 0.0f, 0.0f, 0.0f }, textureData.upperCorner };
    plane[ 5 ] = { origin + height,                    { 0.0f, 0.0f, 0.0f }, { textureData.lowerCorner.x, textureData.upperCorner.y } };

    return plane;
  }

  WallModelLoader::PlaneGroup WallModelLoader::sideToStagedMesh( const Models::Sides& sides, const glm::vec3& origin, const glm::vec3& width ) {
    glm::vec3 bottomRight = origin + width;
    glm::vec3 wallDirection = glm::normalize( bottomRight - origin );
    glm::vec3 wallPerpDirection = glm::rotateZ( wallDirection, glm::radians( 90.0f ) );
    glm::vec3 inverseWallDirection = -1.0f * wallDirection;
    glm::vec3 inverseWallPerpDirection = -1.0f * wallPerpDirection;
    glm::vec3 topRight = bottomRight + ( 0.1f * wallPerpDirection );
    glm::vec3 topLeft = origin + ( 0.1f * wallPerpDirection );
    glm::vec3 upperOrigin = origin + glm::vec3{ 0.0f, 0.0f, 4.0f };

    PlaneGroup planeGroup;
    planeGroup.emplace( "back", getPlane( origin, width, { 0.0f, 0.0f, 4.0f }, sides.back.first ) );
    planeGroup.emplace( "right", getPlane( bottomRight, 0.1f * wallPerpDirection, { 0.0f, 0.0f, 4.0f }, "__top_side" ) );
    planeGroup.emplace( "front", getPlane( topRight, 1.0f * inverseWallDirection, { 0.0f, 0.0f, 4.0f }, sides.front.first ) );
    planeGroup.emplace( "left", getPlane( topLeft, 0.1f * inverseWallPerpDirection, { 0.0f, 0.0f, 4.0f }, "__top_side" ) );
    planeGroup.emplace( "top", getPlane( upperOrigin, wallDirection, 0.1f * wallPerpDirection, "__top_side" ) );
    return planeGroup;
  }

  void WallModelLoader::generateDeferredMeshes() {
    // Generate deferred meshes
    glm::ivec2 cursor{ 0, 0 };
    for( ; cursor.y != dimensions.y; cursor.y++ ) {
      for( ; cursor.x != dimensions.x; cursor.x++ ) {
        Corner& corner = cornerMap[ cursor.y ][ cursor.x ];

        // TODO: Elevation per vertex
        glm::vec3 topLeftCorner = indexToLocation( cursor );

        if( corner.horizontal.model ) {
          corner.horizontal.stagedMesh = sideToStagedMesh( *corner.horizontal.model, topLeftCorner + glm::vec3{ 0.0f, -0.05f, 0.0f }, { 1.0f, 0.0f, 0.0f } );
        }

        if( corner.vertical.model ) {
          corner.vertical.stagedMesh = sideToStagedMesh( *corner.vertical.model, topLeftCorner + glm::vec3{ -0.05f, 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f } );
        }

        // TODO: Diagonal pieces, non-trivial

        fixCorners( cursor );
      }
    }
  }

  void WallModelLoader::addToGenerator( Mesh::IndexedMeshGenerator< Mesh::TexturedVertex >& generator, const PlaneGroup& planeGroup ) {
    for( const auto& pair : planeGroup ) {
      generator.addTriangle( pair.second[ 0 ], pair.second[ 1 ], pair.second[ 2 ] );
      generator.addTriangle( pair.second[ 3 ], pair.second[ 4 ], pair.second[ 5 ] );
    }
  }

  /**
   * All the meshes are ready and corners fixed
   */
  Drawable WallModelLoader::generateDrawable() {
    std::shared_ptr< Texture > generatedTexture = std::make_shared< Texture >( *atlas.generateAtlas() );

    // Result will be a single model with a single mesh built using a mesh generator
    Mesh::IndexedMeshGenerator< Mesh::TexturedVertex > generator;
    for( const auto& row : cornerMap ) {
      for( const auto& corner : row ) {
        if( corner.horizontal.model ) { addToGenerator( generator, corner.horizontal.stagedMesh ); }
        if( corner.vertical.model ) { addToGenerator( generator, corner.vertical.stagedMesh ); }
        if( corner.diagonal.model ) { addToGenerator( generator, corner.diagonal.stagedMesh ); }
        if( corner.reverseDiagonal.model ) { addToGenerator( generator, corner.reverseDiagonal.stagedMesh ); }
      }
    }

    generator.generateNormals();

    return {
      generator.generateMesh(),
      shader,
      std::make_shared< Material >( std::vector< std::shared_ptr< Texture > >{ generatedTexture }, std::vector< std::shared_ptr< Texture > >{}, 0.0f, 1.0f )
    };
  }

  std::shared_ptr< Model > WallModelLoader::get() {
    std::shared_ptr< Model > result = Model::create( "__wallrig", {} );

    for( const auto& level : floorLevels ) {
      // If there are no segments, return null pointer and don't waste any time
      if( level.wallSegments.empty() ) {
        continue;
      }

      // Set state of WallModelLoader for current level
      dimensions = level.dimensions;
      initCornerMap();

      // Walk each line then check joint map to see if vertices need to be stretched to close corners
      for( const auto& segment : level.wallSegments ) {
        // Add texture for front and back to cache
        insertIntoAtlas( segment.faces, atlas );

        // Insert line segment into cornermap
        insertCornerMapSegment( segment );
      }

      generateDeferredMeshes();

      result->addChild( Model::create( "__wall_level", { generateDrawable() } ) );
    }

    return result;
  }

}
