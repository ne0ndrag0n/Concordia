#ifndef WALLCELLBUNDLER
#define WALLCELLBUNDLER

#include "graphics/imagecache.hpp"
#include "graphics/texturecache.hpp"
#include "threading/lockable.hpp"
#include "scripting/wallcell.hpp"
#include <memory>
#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>

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

      std::weak_ptr< WallCellBundler > topNeighbour;
      std::weak_ptr< WallCellBundler > leftNeighbour;

      glm::vec3 center;

      struct SegmentBundle {
        std::shared_ptr< sf::Image > image;
        std::shared_ptr< sf::Image > leftSegment;
        std::shared_ptr< sf::Image > centerSegment;
        std::shared_ptr< sf::Image > rightSegment;
      };

      bool isWallDimensionPresent( std::string& frontPath, std::string& backPath, std::unique_ptr< Scripting::WallCell::Segment >& ptr );
      void newXWallInstance( std::string& frontWallpaper, std::string& backWallpaper );
      void newYWallInstance( std::string& frontWallpaper, std::string& backWallpaper );
      void newDWallInstance( std::string& frontWallpaper, std::string& backWallpaper );
      void newRWallInstance( std::string& frontWallpaper, std::string& backWallpaper );

    public:
      static std::unique_ptr< Model > Piece;

      Threading::Lockable< Scripting::WallCell > hostCellPtr;

      WallCellBundler(
        Threading::Lockable< Scripting::WallCell > hostCell, std::weak_ptr< WallCellBundler > topNeighbour, std::weak_ptr< WallCellBundler > leftNeighbour,
        glm::vec3 center,
        unsigned int currentRotation, TextureCache& hostTextureCache, ImageCache& hostImageCache, unsigned int shader
      );

      std::unique_ptr< Instance > x;
      std::unique_ptr< Instance > y;
      std::unique_ptr< Instance > d;
      std::unique_ptr< Instance > r;

      SegmentBundle getSegmentBundle( const std::string& path, bool useLeft = true, bool useCenter = true, bool useRight = true );
      void render();
    };

  }
}

#endif
