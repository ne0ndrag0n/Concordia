#include "graphics/wallcellbundler.hpp"
#include "graphics/instance/wallinstance.hpp"
#include "graphics/model.hpp"
#include "graphics/imagecache.hpp"
#include "graphics/texturecache.hpp"
#include "graphics/imagebuilder/imagesource.hpp"
#include "graphics/imagebuilder/pathimagesource.hpp"
#include "graphics/imagebuilder/croppeddirectimagesource.hpp"
#include "graphics/material.hpp"
#include <memory>
#include <string>
#include <map>
#include <utility>
#include <SFML/Graphics.hpp>

namespace BlueBear {
  namespace Graphics {

    std::shared_ptr< Model > WallCellBundler::Piece( nullptr );
    const std::string WallCellBundler::WALLATLAS_PATH = "system/models/wall/wallatlas.json";

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

    void WallCellBundler::newXWallInstance( float xPos, float yPos, float floorLevel, std::string& frontWallpaper, std::string& backWallpaper ) {
      x = std::make_shared< Instance >( *WallCellBundler::Piece, shader );

      std::map< std::string, std::unique_ptr< ImageSource > > settings;
      SegmentBundle front = getSegmentBundle( frontWallpaper );
      SegmentBundle back = getSegmentBundle( backWallpaper );

      settings.emplace( std::make_pair( "FrontWall", std::make_unique< DirectImageSource >( *front.image, frontWallpaper ) ) );
      settings.emplace( std::make_pair( "BackWall", std::make_unique< DirectImageSource >( *back.image, backWallpaper ) ) );

      switch( currentRotation ) {
        case 0:
          settings.emplace( std::make_pair( "Side2", std::make_unique< DirectImageSource >( *back.rightSegment, "0xs2 " + backWallpaper ) ) );
          break;
        case 1:
          settings.emplace( std::make_pair( "Side1", std::make_unique< DirectImageSource >( *back.leftSegment, "1xs1 " + backWallpaper ) ) );
          break;
        case 2:
          settings.emplace( std::make_pair( "Side1", std::make_unique< DirectImageSource >( *front.rightSegment, "2xs1 " + frontWallpaper ) ) );
          break;
        case 3:
        default:
          settings.emplace( std::make_pair( "Side2", std::make_unique< DirectImageSource >( *front.leftSegment, "3xs2 " + frontWallpaper ) ) );
      }

      std::shared_ptr< Material > material = std::make_shared< Material >( hostTextureCache.getUsingAtlas( WALLATLAS_PATH, settings ) );

      x->drawable->material = material;
      x->findChildByName( "LeftCorner" )->drawable->material = material;
      x->findChildByName( "RightCorner" )->drawable->material = material;
    }

    void WallCellBundler::newYWallInstance( float xPos, float yPos, float floorLevel, std::string& frontWallpaper, std::string& backWallpaper ) {
      y = std::make_shared< Instance >( *WallCellBundler::Piece, shader );

      std::map< std::string, std::unique_ptr< ImageSource > > settings;
      SegmentBundle front = getSegmentBundle( frontWallpaper );
      SegmentBundle back = getSegmentBundle( backWallpaper );

      switch( currentRotation ) {
        case 0:
          settings.emplace( std::make_pair( "FrontWall", std::make_unique< DirectImageSource >( *front.image, frontWallpaper ) ) );
          settings.emplace( std::make_pair( "Side2", std::make_unique< DirectImageSource >( *front.leftSegment, "0ys2 " + frontWallpaper ) ) );
          break;
        case 1:
          settings.emplace( std::make_pair( "BackWall", std::make_unique< DirectImageSource >( *back.image, backWallpaper ) ) );
          settings.emplace( std::make_pair( "Side2", std::make_unique< DirectImageSource >( *back.rightSegment, "1ys2 " + backWallpaper ) ) );
          break;
        case 2:
          settings.emplace( std::make_pair( "BackWall", std::make_unique< DirectImageSource >( *back.image, backWallpaper ) ) );
          settings.emplace( std::make_pair( "Side1", std::make_unique< DirectImageSource >( *back.leftSegment, "2ys1 " + backWallpaper ) ) );
          break;
        case 3:
        default:
          settings.emplace( std::make_pair( "FrontWall", std::make_unique< DirectImageSource >( *front.image, frontWallpaper ) ) );
          settings.emplace( std::make_pair( "Side1", std::make_unique< DirectImageSource >( *front.rightSegment, "3ys1 " + frontWallpaper ) ) );
      }

      std::shared_ptr< Material > material = std::make_shared< Material >( hostTextureCache.getUsingAtlas( WALLATLAS_PATH, settings ) );

      y->drawable->material = material;
      y->findChildByName( "LeftCorner" )->drawable->material = material;
      y->findChildByName( "RightCorner" )->drawable->material = material;
    }

  }
}
