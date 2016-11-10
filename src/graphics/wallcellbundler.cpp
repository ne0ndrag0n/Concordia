#include "graphics/wallcellbundler.hpp"
#include "graphics/instance/wallinstance.hpp"
#include "graphics/model.hpp"
#include "graphics/imagecache.hpp"
#include "graphics/texturecache.hpp"
#include "graphics/imagebuilder/pathimagesource.hpp"
#include "graphics/imagebuilder/croppeddirectimagesource.hpp"
#include <memory>
#include <string>
#include <SFML/Graphics.hpp>

namespace BlueBear {
  namespace Graphics {

    std::shared_ptr< Model > WallCellBundler::Piece( nullptr );

    WallCellBundler::WallCellBundler( unsigned int currentRotation, TextureCache& hostTextureCache, ImageCache& hostImageCache, unsigned int shader ) :
     currentRotation( currentRotation ),
     hostTextureCache( hostTextureCache ),
     hostImageCache( hostImageCache ),
     shader( shader ) {}

    void WallCellBundler::render() {
      if( x ) {
        x->drawEntity();
      }

      if( y ) {
        y->drawEntity();
      }
    }

    WallCellBundler::SegmentBundle WallCellBundler::getSegmentBundle( const std::string& path ) {
      WallCellBundler::SegmentBundle side;

      PathImageSource pis( path );
      side.image = hostImageCache.getImage( pis );

      // Slice images into their left and right segments
      const auto originalSize = side.image->getSize();

      CroppedDirectImageSource left( *side.image, 0, 0, 6, 192, path );
      CroppedDirectImageSource right( *side.image, originalSize.x - 7, 0, 6, 192, path );

      side.leftSegment = hostImageCache.getImage( left );
      side.rightSegment = hostImageCache.getImage( right );

      return side;
    }

    void WallCellBundler::newXWallInstance( float x, float y, float floorLevel, std::string& frontWallpaper, std::string& backWallpaper ) {

    }

  }
}
