#ifndef WALLINSTANCE
#define WALLINSTANCE

#include "graphics/instance/instance.hpp"
#include "graphics/texturecache.hpp"
#include "graphics/imagecache.hpp"
#include <SFML/Graphics.hpp>
#include <string>
#include <memory>
#include <unordered_map>
#include <functional>

namespace BlueBear {
  namespace Graphics {
    class Model;

    class WallInstance : public Instance {
      static const std::string WALLATLAS_PATH;

    public:
      struct WallpaperSide {
        std::shared_ptr< sf::Image > image;
        std::shared_ptr< sf::Image > leftSegment;
        std::shared_ptr< sf::Image > rightSegment;
        std::string path;
      };

      WallInstance( const Model& model, GLuint shaderProgram, TextureCache& hostTextureCache, ImageCache& hostImageCache );
      void setFrontWallpaper( const std::string& path );
      void setBackWallpaper( const std::string& path );

      void setWallpaper( const std::string& frontPath, const std::string& backPath );

      void selectMaterial( unsigned int rotation );

    protected:
      std::function< void( unsigned int, std::map< std::string, std::unique_ptr< ImageSource > >& ) > selectedRotationFunction;

      TextureCache& hostTextureCache;
      ImageCache& hostImageCache;
      WallpaperSide front;
      WallpaperSide back;

      void setWallpaper( const std::string& path, WallpaperSide& side );
      virtual void setRotationAttributes( unsigned int rotation, std::map< std::string, std::unique_ptr< ImageSource > >& settings ) = 0;
    };

  }
}

#endif
