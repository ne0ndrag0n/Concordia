#include "graphics/instance/xwallinstance.hpp"
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

    std::shared_ptr< Model > XWallInstance::Piece( nullptr );
    std::shared_ptr< Model > XWallInstance::EdgePiece( nullptr );

    XWallInstance::XWallInstance( GLuint shaderProgram, TextureCache& hostTextureCache, ImageCache& hostImageCache ) : WallInstance::WallInstance( *XWallInstance::Piece, shaderProgram, hostTextureCache, hostImageCache ) {}

    void XWallInstance::setRotationAttributes( unsigned int rotation, std::map< std::string, std::unique_ptr< ImageSource > >& settings ) {
      switch( rotation ) {
        case 0:
          settings.emplace( std::make_pair( "Side2", std::make_unique< DirectImageSource >( *back.rightSegment, "0xs2 " + back.path ) ) );
          break;
        case 1:
          settings.emplace( std::make_pair( "Side1", std::make_unique< DirectImageSource >( *back.leftSegment, "1xs1 " + back.path ) ) );
          break;
        case 2:
          settings.emplace( std::make_pair( "Side1", std::make_unique< DirectImageSource >( *front.rightSegment, "2xs1 " + front.path ) ) );
          break;
        case 3:
        default:
          settings.emplace( std::make_pair( "Side2", std::make_unique< DirectImageSource >( *front.leftSegment, "3xs2 " + front.path ) ) );
      }
    }

  }
}
