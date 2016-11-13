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
      static const std::string WALLATLAS_PATH;

      unsigned int shader;
      unsigned int currentRotation;
      TextureCache& hostTextureCache;
      ImageCache& hostImageCache;

      struct SegmentBundle {
        std::shared_ptr< sf::Image > image;
        std::shared_ptr< sf::Image > leftSegment;
        std::shared_ptr< sf::Image > centerSegment;
        std::shared_ptr< sf::Image > rightSegment;
      };

      SegmentBundle getSegmentBundle( const std::string& path, bool useLeft = true, bool useCenter = true, bool useRight = true );

    public:
      static std::unique_ptr< Model > Piece;

      WallCellBundler( unsigned int currentRotation, TextureCache& hostTextureCache, ImageCache& hostImageCache, unsigned int shader );

      std::unique_ptr< Instance > x;
      std::unique_ptr< Instance > y;
      std::unique_ptr< Instance > d;
      std::unique_ptr< Instance > r;

      void render();
      void newXWallInstance( float xPos, float yPos, float floorLevel, std::string& frontWallpaper, std::string& backWallpaper, std::shared_ptr< WallCellBundler > topNeighbour );
      void newYWallInstance( float xPos, float yPos, float floorLevel, std::string& frontWallpaper, std::string& backWallpaper, std::shared_ptr< WallCellBundler > leftNeighbour );
      void newDWallInstance( float xPos, float yPos, float floorLevel, std::string& frontWallpaper, std::string& backWallpaper );
      void newRWallInstance( float xPos, float yPos, float floorLevel, std::string& frontWallpaper, std::string& backWallpaper );
    };

  }
}

#endif
