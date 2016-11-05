#include "graphics/instance/wallinstance.hpp"
#include "graphics/instance/instance.hpp"
#include "graphics/imagebuilder/imagesource.hpp"
#include "graphics/imagebuilder/directimagesource.hpp"
#include "graphics/imagebuilder/croppeddirectimagesource.hpp"
#include "graphics/imagebuilder/pathimagesource.hpp"
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

      PathImageSource pis( path );
      side.image = hostImageCache.getImage( pis );

      // Slice images into their left and right segments
      const auto originalSize = side.image->getSize();

      CroppedDirectImageSource left( *side.image, 0, 0, 6, 192, path );
      CroppedDirectImageSource right( *side.image, originalSize.x - 7, 0, 6, 192, path );

      side.leftSegment = hostImageCache.getImage( left );
      side.rightSegment = hostImageCache.getImage( right );
    }

    /**
     *
     */
    void WallInstance::selectMaterial( unsigned int rotation ) {
      std::map< std::string, std::unique_ptr< ImageSource > > settings;

      // Apply based on rotation
      setRotationAttributes( rotation, settings );

      std::shared_ptr< Texture > texture = hostTextureCache.getUsingAtlas( WALLATLAS_PATH, settings );
      drawables.at( "Wall" ).material = std::make_shared< Material >( texture );
    }

  }
}
