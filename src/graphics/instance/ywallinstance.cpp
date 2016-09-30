#include "graphics/instance/ywallinstance.hpp"
#include "graphics/instance/wallinstance.hpp"
#include "graphics/model.hpp"
#include "graphics/imagebuilder/imagesource.hpp"
#include "graphics/imagebuilder/directimagesource.hpp"
#include <memory>
#include <map>
#include <string>

namespace BlueBear {
  namespace Graphics {

    YWallInstance::YWallInstance( const Model& model, GLuint shaderProgram, TextureCache& hostTextureCache ) : WallInstance::WallInstance( model, shaderProgram, hostTextureCache ) {}

    void YWallInstance::setRotationAttributes( unsigned int rotation, std::map< std::string, std::unique_ptr< ImageSource > >& settings ) {
      switch( rotation ) {
        case 0:
          settings.emplace( std::make_pair( "Side2", std::make_unique< DirectImageSource >( front.leftSegment, "0ry " + front.path ) ) );
          break;
        case 1:
          settings.emplace( std::make_pair( "Side2", std::make_unique< DirectImageSource >( front.rightSegment, "1ry " + front.path ) ) );
          break;
        case 2:
          settings.emplace( std::make_pair( "Side1", std::make_unique< DirectImageSource >( front.leftSegment, "2ry " + front.path ) ) );
          break;
        case 3:
        default:
          settings.emplace( std::make_pair( "Side1", std::make_unique< DirectImageSource >( front.rightSegment, "3ry " + front.path ) ) );
      }
    }

  }
}
