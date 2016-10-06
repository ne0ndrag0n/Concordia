#include "graphics/instance/wallinstance.hpp"
#include "graphics/instance/instance.hpp"
#include "graphics/imagebuilder/imagesource.hpp"
#include "graphics/imagebuilder/directimagesource.hpp"
#include "graphics/texturecache.hpp"
#include "graphics/imagecache.hpp"
#include "graphics/model.hpp"
#include "graphics/texture.hpp"
#include "graphics/material.hpp"
#include "exceptions/cannotloadfile.hpp"
#include <SFML/Graphics.hpp>
#include <string>
#include <GL/glew.h>
#include <memory>
#include <utility>

namespace BlueBear {
  namespace Graphics {

    const std::string WallInstance::WALLATLAS_PATH = "system/models/wall/wallatlas.json";
    WallInstance::ImageMap WallInstance::imageMap;

    WallInstance::WallInstance( const Model& model, GLuint shaderProgram, TextureCache& hostTextureCache, ImageCache& hostImageCache ) :
      Instance::Instance( model, shaderProgram ),
      hostTextureCache( hostTextureCache ),
      hostImageCache( hostImageCache ) {}


    void WallInstance::setFrontWallpaper( const std::string& path ) {
      setWallpaper( path, front );
    }

    void WallInstance::setBackWallpaper( const std::string& path ) {
      setWallpaper( path, back );
    }

    void WallInstance::setWallpaper( const std::string& frontPath, const std::string& backPath ) {
      setWallpaper( frontPath, front );
      setWallpaper( backPath, back );
    }

    void WallInstance::setWallpaper( const std::string& path, WallpaperSide& side ) {
      side.path = path;

      side.image = getImage( path );

      // Slice images into their left and right segments
      const auto originalSize = side.image->getSize();

      side.leftSegment = std::make_shared< sf::Image >();
      side.leftSegment->create( 44, 1020 );
      side.leftSegment->copy( *side.image, 0, 0, { 0, 0, 44, 1020 } );

      side.rightSegment = std::make_shared< sf::Image >();
      side.rightSegment->create( 44, 1020 );
      side.rightSegment->copy( *side.image, 0, 0, { (int)originalSize.x - 45, 0, 44, 1020 } );
    }

    /**
     *
     */
    void WallInstance::selectMaterial( unsigned int rotation ) {
      std::map< std::string, std::unique_ptr< ImageSource > > settings;

      settings.emplace( std::make_pair( "FrontWall", std::make_unique< DirectImageSource >( *front.image, front.path ) ) );
      settings.emplace( std::make_pair( "BackWall", std::make_unique< DirectImageSource >( *back.image, back.path ) ) );

      // Apply based on rotation
      // TODO: These only work on the Y-segments. Differentiate.
      setRotationAttributes( rotation, settings );

      std::shared_ptr< Texture > texture = hostTextureCache.getUsingAtlas( WALLATLAS_PATH, settings );
      drawables.at( "Wall" ).material = std::make_shared< Material >( texture );
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
