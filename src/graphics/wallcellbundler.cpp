#include "graphics/wallcellbundler.hpp"
#include "graphics/model.hpp"
#include "graphics/imagecache.hpp"
#include "graphics/texturecache.hpp"
#include "graphics/instance/instance.hpp"
#include "graphics/imagebuilder/imagesource.hpp"
#include "graphics/imagebuilder/pathimagesource.hpp"
#include "graphics/imagebuilder/croppeddirectimagesource.hpp"
#include "graphics/material.hpp"
#include <memory>
#include <string>
#include <map>
#include <utility>
#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>

namespace BlueBear {
  namespace Graphics {

    std::unique_ptr< Model > WallCellBundler::Piece( nullptr );
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

    WallCellBundler::SegmentBundle WallCellBundler::getSegmentBundle( const std::string& path, bool useLeft, bool useCenter, bool useRight ) {
      WallCellBundler::SegmentBundle side;

      PathImageSource pis( path );
      side.image = hostImageCache.getImage( pis );

      // Slice images into their left and right segments
      const auto originalSize = side.image->getSize();

      if( useLeft ) {
        CroppedDirectImageSource left( *side.image, 0, 0, 6, 192, path );
        side.leftSegment = hostImageCache.getImage( left );
      }

      if( useCenter ) {
        CroppedDirectImageSource center( *side.image, 6, 0, 36, 192, path );
        side.centerSegment = hostImageCache.getImage( center );
      }

      if( useRight ) {
        // This was -7. I'm not sure why. It makes far more sense to be imageSize.x - 6.
        CroppedDirectImageSource right( *side.image, originalSize.x - 6, 0, 6, 192, path );
        side.rightSegment = hostImageCache.getImage( right );
      }

      return side;
    }

    void WallCellBundler::newXWallInstance( float xPos, float yPos, float floorLevel, std::string& frontWallpaper, std::string& backWallpaper, std::shared_ptr< WallCellBundler > topNeighbour ) {
      x = std::make_unique< Instance >( *WallCellBundler::Piece, shader );

      std::map< std::string, std::unique_ptr< ImageSource > > settings;
      SegmentBundle front = getSegmentBundle( frontWallpaper );
      SegmentBundle back = getSegmentBundle( backWallpaper );

      switch( currentRotation ) {
        case 0:
          //settings.emplace( std::make_pair( "BackWall", std::make_unique< DirectImageSource >( *back.image, backWallpaper ) ) );
          settings.emplace( std::make_pair( "BackWallLeft", std::make_unique< DirectImageSource >( *back.leftSegment, "0xl " + backWallpaper ) ) );
          settings.emplace( std::make_pair( "BackWallCenter", std::make_unique< DirectImageSource >( *back.centerSegment, "0xc " + backWallpaper ) ) );
          settings.emplace( std::make_pair( "BackWallRight", std::make_unique< DirectImageSource >( *back.rightSegment, "0xr " + backWallpaper ) ) );
          settings.emplace( std::make_pair( "Side2", std::make_unique< DirectImageSource >( *back.rightSegment, "0xs2 " + backWallpaper ) ) );
          break;
        case 1:
          //settings.emplace( std::make_pair( "BackWall", std::make_unique< DirectImageSource >( *back.image, backWallpaper ) ) );
          settings.emplace( std::make_pair( "BackWallLeft", std::make_unique< DirectImageSource >( *back.leftSegment, "1xl " + backWallpaper ) ) );
          settings.emplace( std::make_pair( "BackWallCenter", std::make_unique< DirectImageSource >( *back.centerSegment, "1xc " + backWallpaper ) ) );
          settings.emplace( std::make_pair( "BackWallRight", std::make_unique< DirectImageSource >( *back.rightSegment, "1xr " + backWallpaper ) ) );
          settings.emplace( std::make_pair( "Side1", std::make_unique< DirectImageSource >( *back.leftSegment, "1xs1 " + backWallpaper ) ) );
          break;
        case 2:
          //settings.emplace( std::make_pair( "FrontWall", std::make_unique< DirectImageSource >( *front.image, frontWallpaper ) ) );
          settings.emplace( std::make_pair( "FrontWallLeft", std::make_unique< DirectImageSource >( *front.leftSegment, "2xl " + frontWallpaper ) ) );
          settings.emplace( std::make_pair( "FrontWallCenter", std::make_unique< DirectImageSource >( *front.centerSegment, "2xc " + frontWallpaper ) ) );
          settings.emplace( std::make_pair( "FrontWallRight", std::make_unique< DirectImageSource >( *front.rightSegment, "2xr " + frontWallpaper ) ) );
          settings.emplace( std::make_pair( "Side1", std::make_unique< DirectImageSource >( *front.rightSegment, "2xs1 " + frontWallpaper ) ) );
          break;
        case 3:
        default:
          //settings.emplace( std::make_pair( "FrontWall", std::make_unique< DirectImageSource >( *front.image, frontWallpaper ) ) );
          settings.emplace( std::make_pair( "FrontWallLeft", std::make_unique< DirectImageSource >( *front.leftSegment, "3xl " + frontWallpaper ) ) );
          settings.emplace( std::make_pair( "FrontWallCenter", std::make_unique< DirectImageSource >( *front.centerSegment, "3xc " + frontWallpaper ) ) );
          settings.emplace( std::make_pair( "FrontWallRight", std::make_unique< DirectImageSource >( *front.rightSegment, "3xr " + frontWallpaper ) ) );
          settings.emplace( std::make_pair( "Side2", std::make_unique< DirectImageSource >( *front.leftSegment, "3xs2 " + frontWallpaper ) ) );
      }

      glm::vec3 position( xPos, yPos + 0.9f, floorLevel );
      if ( currentRotation == 0 || currentRotation == 1 ) {
        position.y = position.y + 0.1f;
      }

      x->setPosition( position );
      x->setRotationAngle( glm::radians( 180.0f ) );

      std::shared_ptr< Material > material = std::make_shared< Material >( hostTextureCache.getUsingAtlas( WALLATLAS_PATH, settings ) );

      x->drawable->material = material;
      x->findChildByName( "LeftCorner" )->drawable->material = material;
      x->findChildByName( "RightCorner" )->drawable->material = material;
    }

    void WallCellBundler::newYWallInstance( float xPos, float yPos, float floorLevel, std::string& frontWallpaper, std::string& backWallpaper, std::shared_ptr< WallCellBundler > leftNeighbour ) {
      y = std::make_unique< Instance >( *WallCellBundler::Piece, shader );

      std::map< std::string, std::unique_ptr< ImageSource > > settings;
      SegmentBundle front = getSegmentBundle( frontWallpaper );
      SegmentBundle back = getSegmentBundle( backWallpaper );

      switch( currentRotation ) {
        case 0:
          //settings.emplace( std::make_pair( "FrontWall", std::make_unique< DirectImageSource >( *front.image, frontWallpaper ) ) );
          settings.emplace( std::make_pair( "FrontWallLeft", std::make_unique< DirectImageSource >( *front.leftSegment, "0yl " + frontWallpaper ) ) );
          settings.emplace( std::make_pair( "FrontWallCenter", std::make_unique< DirectImageSource >( *front.centerSegment, "0yc " + frontWallpaper ) ) );
          settings.emplace( std::make_pair( "FrontWallRight", std::make_unique< DirectImageSource >( *front.rightSegment, "0yr " + frontWallpaper ) ) );
          settings.emplace( std::make_pair( "Side2", std::make_unique< DirectImageSource >( *front.leftSegment, "0ys2 " + frontWallpaper ) ) );
          break;
        case 1:
          //settings.emplace( std::make_pair( "BackWall", std::make_unique< DirectImageSource >( *back.image, backWallpaper ) ) );
          settings.emplace( std::make_pair( "BackWallLeft", std::make_unique< DirectImageSource >( *back.leftSegment, "1yl " + backWallpaper ) ) );
          settings.emplace( std::make_pair( "BackWallCenter", std::make_unique< DirectImageSource >( *back.centerSegment, "1yc " + backWallpaper ) ) );
          settings.emplace( std::make_pair( "BackWallRight", std::make_unique< DirectImageSource >( *back.rightSegment, "1yr " + backWallpaper ) ) );
          settings.emplace( std::make_pair( "Side2", std::make_unique< DirectImageSource >( *back.rightSegment, "1ys2 " + backWallpaper ) ) );
          break;
        case 2:
          //settings.emplace( std::make_pair( "BackWall", std::make_unique< DirectImageSource >( *back.image, backWallpaper ) ) );
          settings.emplace( std::make_pair( "BackWallLeft", std::make_unique< DirectImageSource >( *back.leftSegment, "2yl " + backWallpaper ) ) );
          settings.emplace( std::make_pair( "BackWallCenter", std::make_unique< DirectImageSource >( *back.centerSegment, "2yc " + backWallpaper ) ) );
          settings.emplace( std::make_pair( "BackWallRight", std::make_unique< DirectImageSource >( *back.rightSegment, "2yr " + backWallpaper ) ) );
          settings.emplace( std::make_pair( "Side1", std::make_unique< DirectImageSource >( *back.leftSegment, "2ys1 " + backWallpaper ) ) );
          break;
        case 3:
        default:
          //settings.emplace( std::make_pair( "FrontWall", std::make_unique< DirectImageSource >( *front.image, frontWallpaper ) ) );
          settings.emplace( std::make_pair( "FrontWallLeft", std::make_unique< DirectImageSource >( *front.leftSegment, "3yl " + frontWallpaper ) ) );
          settings.emplace( std::make_pair( "FrontWallCenter", std::make_unique< DirectImageSource >( *front.centerSegment, "3yc " + frontWallpaper ) ) );
          settings.emplace( std::make_pair( "FrontWallRight", std::make_unique< DirectImageSource >( *front.rightSegment, "3yr " + frontWallpaper ) ) );
          settings.emplace( std::make_pair( "Side1", std::make_unique< DirectImageSource >( *front.rightSegment, "3ys1 " + frontWallpaper ) ) );
      }

      glm::vec3 position( xPos - 0.9f, yPos, floorLevel );
      if ( currentRotation == 1 || currentRotation == 2 ) {
        position.x = position.x - 0.1f;
      }

      y->setPosition( position );
      y->setRotationAngle( glm::radians( -90.0f ) );

      std::shared_ptr< Material > material = std::make_shared< Material >( hostTextureCache.getUsingAtlas( WALLATLAS_PATH, settings ) );

      y->drawable->material = material;
      y->findChildByName( "LeftCorner" )->drawable->material = material;
      y->findChildByName( "RightCorner" )->drawable->material = material;
    }

  }
}
