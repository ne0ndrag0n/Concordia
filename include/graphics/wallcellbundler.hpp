#ifndef WALLCELLBUNDLER
#define WALLCELLBUNDLER

#include "graphics/imagecache.hpp"
#include "graphics/texturecache.hpp"
#include <memory>
#include <SFML/Graphics.hpp>

namespace BlueBear {
  namespace Graphics {
    class Instance;
    class Model;

    class WallCellBundler {
      unsigned int shader;
      unsigned int currentRotation;
      TextureCache& hostTextureCache;
      ImageCache& hostImageCache;

      struct SegmentBundle {
        std::shared_ptr< sf::Image > image;
        std::shared_ptr< sf::Image > leftSegment;
        std::shared_ptr< sf::Image > rightSegment;
      };

      SegmentBundle getSegmentBundle( const std::string& path );

    public:
      static std::shared_ptr< Model > Piece;

      WallCellBundler( unsigned int currentRotation, TextureCache& hostTextureCache, ImageCache& hostImageCache, unsigned int shader );

      std::shared_ptr< Instance > x;
      std::shared_ptr< Instance > y;
      std::shared_ptr< Instance > d;
      std::shared_ptr< Instance > r;

      void render();
      void newXWallInstance( float x, float y, float floorLevel, std::string& frontWallpaper, std::string& backWallpaper );
      void newYWallInstance( float x, float y, float floorLevel, std::string& frontWallpaper, std::string& backWallpaper );
      void newDWallInstance( float x, float y, float floorLevel, std::string& frontWallpaper, std::string& backWallpaper );
      void newRWallInstance( float x, float y, float floorLevel, std::string& frontWallpaper, std::string& backWallpaper );
    };

  }
}

#endif
