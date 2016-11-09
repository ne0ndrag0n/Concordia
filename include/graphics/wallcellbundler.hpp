#ifndef WALLCELLBUNDLER
#define WALLCELLBUNDLER

#include <memory>

namespace BlueBear {
  namespace Graphics {
    class WallInstance;
    class Model;

    class WallCellBundler {
      unsigned int currentRotation;
    public:
      static std::shared_ptr< Model > Piece;

      WallCellBundler( unsigned int currentRotation );
      std::shared_ptr< WallInstance > x;
      std::shared_ptr< WallInstance > y;
      std::shared_ptr< WallInstance > d;
      std::shared_ptr< WallInstance > r;

      void render();
      void newXWallInstance( float x, float y, float floorLevel, std::string& frontWallpaper, std::string& backWallpaper );
      void newYWallInstance( float x, float y, float floorLevel, std::string& frontWallpaper, std::string& backWallpaper );
      void newDWallInstance( float x, float y, float floorLevel, std::string& frontWallpaper, std::string& backWallpaper );
      void newRWallInstance( float x, float y, float floorLevel, std::string& frontWallpaper, std::string& backWallpaper );
    };

  }
}

#endif
