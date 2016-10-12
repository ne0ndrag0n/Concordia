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

namespace BlueBear {
  namespace Graphics {

    std::shared_ptr< Model > YWallInstance::Piece( nullptr );
    std::shared_ptr< Model > YWallInstance::EdgePiece( nullptr );

    YWallInstance::YWallInstance( GLuint shaderProgram, TextureCache& hostTextureCache, ImageCache& hostImageCache ) : WallInstance::WallInstance( *YWallInstance::Piece, shaderProgram, hostTextureCache, hostImageCache ) {}

    void YWallInstance::setRotationAttributes( unsigned int rotation, std::map< std::string, std::unique_ptr< ImageSource > >& settings ) {
      switch( rotation ) {
        case 0:
          settings.emplace( std::make_pair( "Side2", std::make_unique< DirectImageSource >( *front.leftSegment, "0ys2 " + front.path ) ) );
          break;
        case 1:
          settings.emplace( std::make_pair( "Side2", std::make_unique< DirectImageSource >( *back.rightSegment, "1ys2 " + back.path ) ) );
          break;
        case 2:
          settings.emplace( std::make_pair( "Side1", std::make_unique< DirectImageSource >( *back.leftSegment, "2ys1 " + back.path ) ) );
          break;
        case 3:
        default:
          settings.emplace( std::make_pair( "Side1", std::make_unique< DirectImageSource >( *front.rightSegment, "3ys1 " + front.path ) ) );
      }
    }

  }
}