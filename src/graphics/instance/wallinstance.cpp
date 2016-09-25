#include "graphics/instance/wallinstance.hpp"
#include "graphics/instance/instance.hpp"
#include "graphics/texturecache.hpp"
#include "graphics/model.hpp"
#include "exceptions/cannotloadfile.hpp"
#include <SFML/Graphics.hpp>
#include <string>
#include <GL/glew.h>
#include <memory>
#include <utility>

namespace BlueBear {
  namespace Graphics {

    WallInstance::ImageMap WallInstance::imageMap;

    WallInstance::WallInstance( const Model& model, GLuint shaderProgram, TextureCache& hostTextureCache ) :
      Instance::Instance( model, shaderProgram ), hostTextureCache( hostTextureCache ) {}


    void WallInstance::setFrontWallpaper( const std::string& path ) {
      setWallpaper( path, front );
    }

    void WallInstance::setBackWallpaper( const std::string& path ) {
      setWallpaper( path, back );
    }

    void WallInstance::setWallpaper( const std::string& path, WallpaperSide& side ) {
      side.path = path;
      side.image = getImage( path );
    }

    std::shared_ptr< sf::Image > WallInstance::getImage( const std::string& path ) {
      auto pair = imageMap.find( path );

      if( pair == imageMap.end() ) {
        // Create and return
        auto image = std::make_shared< sf::Image >();
        if( !image->loadFromFile( path ) ) {
          throw Exceptions::CannotLoadFileException();
        }

        return imageMap[ path ] = image;
      } else {
        return pair->second;
      }
    }

  }
}
