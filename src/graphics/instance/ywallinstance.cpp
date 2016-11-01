#include "graphics/instance/ywallinstance.hpp"
#include "graphics/instance/wallinstance.hpp"
#include "graphics/model.hpp"
#include "graphics/imagebuilder/imagesource.hpp"
#include "graphics/imagebuilder/directimagesource.hpp"
#include "graphics/texturecache.hpp"
#include "graphics/imagecache.hpp"
#include <memory>
#include <map>
#include <string>
#include <functional>

namespace BlueBear {
  namespace Graphics {

    std::shared_ptr< Model > YWallInstance::Piece( nullptr );
    std::shared_ptr< Model > YWallInstance::EdgePiece( nullptr );

    YWallInstance::YWallInstance( GLuint shaderProgram, TextureCache& hostTextureCache, ImageCache& hostImageCache, bool edgePiece ) :
      WallInstance::WallInstance( edgePiece ? *YWallInstance::EdgePiece : *YWallInstance::Piece, shaderProgram, hostTextureCache, hostImageCache ) {
        selectedRotationFunction = std::bind( edgePiece ? &YWallInstance::setRotationAttributesEdge : &YWallInstance::setRotationAttributes, this, std::placeholders::_1, std::placeholders::_2 );
    }

    /**
     * Set the texture atlas attributes per rotation
     */
    void YWallInstance::setRotationAttributes( unsigned int rotation, std::map< std::string, std::unique_ptr< ImageSource > >& settings ) {

      switch( rotation ) {
        case 0:
          settings.emplace( std::make_pair( "FrontWall", std::make_unique< DirectImageSource >( *front.image, front.path ) ) );
          settings.emplace( std::make_pair( "Side2", std::make_unique< DirectImageSource >( *front.leftSegment, "0ys2 " + front.path ) ) );
          break;
        case 1:
          settings.emplace( std::make_pair( "BackWall", std::make_unique< DirectImageSource >( *back.image, back.path ) ) );
          settings.emplace( std::make_pair( "Side2", std::make_unique< DirectImageSource >( *back.rightSegment, "1ys2 " + back.path ) ) );
          break;
        case 2:
          settings.emplace( std::make_pair( "BackWall", std::make_unique< DirectImageSource >( *back.image, back.path ) ) );
          settings.emplace( std::make_pair( "Side1", std::make_unique< DirectImageSource >( *back.leftSegment, "2ys1 " + back.path ) ) );
          break;
        case 3:
        default:
          settings.emplace( std::make_pair( "FrontWall", std::make_unique< DirectImageSource >( *front.image, front.path ) ) );
          settings.emplace( std::make_pair( "Side1", std::make_unique< DirectImageSource >( *front.rightSegment, "3ys1 " + front.path ) ) );
      }
    }

    void YWallInstance::setRotationAttributesEdge( unsigned int rotation, std::map< std::string, std::unique_ptr< ImageSource > >& settings ) {
      // Similar but not identical code - A different face is seen by the player on certain rotations of the edge piece.

      switch( rotation ) {
        case 0:
          // Back faces the player - set it to the front image
          settings.emplace( std::make_pair( "BackWall", std::make_unique< DirectImageSource >( *front.image, front.path ) ) );
          settings.emplace( std::make_pair( "Side1", std::make_unique< DirectImageSource >( *front.leftSegment, "0ys1 " + front.path ) ) );
          settings.emplace( std::make_pair( "BackWallExtended", std::make_unique< DirectImageSource >( *front.rightSegment, "0yb " + front.path ) ) );
          break;
        case 1:
          // Front faces the player - set it to the back image
          settings.emplace( std::make_pair( "FrontWall", std::make_unique< DirectImageSource >( *back.image, back.path ) ) );
          settings.emplace( std::make_pair( "Side1", std::make_unique< DirectImageSource >( *back.rightSegment, "1ys1 " + back.path ) ) );
          settings.emplace( std::make_pair( "FrontWallExtended", std::make_unique< DirectImageSource >( *back.leftSegment, "1yf " + back.path ) ) );
          break;
        case 2:
          settings.emplace( std::make_pair( "BackWall", std::make_unique< DirectImageSource >( *back.image, back.path ) ) );
          settings.emplace( std::make_pair( "Side1", std::make_unique< DirectImageSource >( *back.leftSegment, "2ys1 " + back.path ) ) );
          settings.emplace( std::make_pair( "BackWallExtended", std::make_unique< DirectImageSource >( *back.rightSegment, "2yb " + back.path ) ) );
          break;
        case 3:
        default:
          settings.emplace( std::make_pair( "FrontWall", std::make_unique< DirectImageSource >( *front.image, front.path ) ) );
          settings.emplace( std::make_pair( "Side1", std::make_unique< DirectImageSource >( *front.rightSegment, "3ys1 " + front.path ) ) );
          settings.emplace( std::make_pair( "FrontWallExtended", std::make_unique< DirectImageSource >( *front.leftSegment, "3yf " + front.path ) ) );
      }
    }

  }
}
