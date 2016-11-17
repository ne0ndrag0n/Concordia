#ifndef WALLCELLBUNDLER
#define WALLCELLBUNDLER

#include "containers/collection3d.hpp"
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
      glm::vec3 counter;

      struct SegmentBundle {
        std::shared_ptr< sf::Image > image;
        std::shared_ptr< sf::Image > leftSegment;
        std::shared_ptr< sf::Image > centerSegment;
        std::shared_ptr< sf::Image > rightSegment;
      };

      bool isWallDimensionPresent( std::string& frontPath, std::string& backPath, std::unique_ptr< Scripting::WallCell::Segment >& ptr );
      void newXWallInstance( Containers::Collection3D< std::shared_ptr< WallCellBundler > >& hostCollection, std::string& frontWallpaper, std::string& backWallpaper );
      void newYWallInstance( Containers::Collection3D< std::shared_ptr< WallCellBundler > >& hostCollection, std::string& frontWallpaper, std::string& backWallpaper );
      void newDWallInstance( Containers::Collection3D< std::shared_ptr< WallCellBundler > >& hostCollection, std::string& frontWallpaper, std::string& backWallpaper );
      void newRWallInstance( Containers::Collection3D< std::shared_ptr< WallCellBundler > >& hostCollection, std::string& frontWallpaper, std::string& backWallpaper );

      std::shared_ptr< WallCellBundler > safeGetBundler( Containers::Collection3D< std::shared_ptr< WallCellBundler > >& hostCollection, int x, int y, int z );

    public:
      static std::unique_ptr< Model > Piece;
      static float xOrigin;
      static float yOrigin;

      Threading::Lockable< Scripting::WallCell > hostCellPtr;

      WallCellBundler(
        Threading::Lockable< Scripting::WallCell > hostCell,
        Containers::Collection3D< std::shared_ptr< WallCellBundler > >& hostCollection,
        glm::vec3 counter,
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
