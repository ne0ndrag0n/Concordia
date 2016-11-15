#include "graphics/wallcellbundler.hpp"
#include "graphics/model.hpp"
#include "graphics/imagecache.hpp"
#include "graphics/texturecache.hpp"
#include "graphics/instance/instance.hpp"
#include "graphics/imagebuilder/imagesource.hpp"
#include "graphics/imagebuilder/pathimagesource.hpp"
#include "graphics/imagebuilder/croppeddirectimagesource.hpp"
#include "graphics/material.hpp"
#include "threading/lockable.hpp"
#include "scripting/wallcell.hpp"
#include "scripting/wallpaper.hpp"
#include "log.hpp"
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

    WallCellBundler::WallCellBundler( Threading::Lockable< Scripting::WallCell > hostCell, std::weak_ptr< WallCellBundler > topNeighbour, std::weak_ptr< WallCellBundler > leftNeighbour, glm::vec3 center, unsigned int currentRotation, TextureCache& hostTextureCache, ImageCache& hostImageCache, unsigned int shader ) :
     currentRotation( currentRotation ),
     hostTextureCache( hostTextureCache ),
     hostImageCache( hostImageCache ),
     shader( shader ),
     hostCellPtr( hostCell ),
     topNeighbour( topNeighbour ),
     leftNeighbour( leftNeighbour ),
     center( center ) {

       std::string frontPath;
       std::string backPath;

       if( hostCellPtr.lock< bool >( [ & ]( Scripting::WallCell& wallCell ) { return isWallDimensionPresent( frontPath, backPath, wallCell.x ); } ) ) {
         newXWallInstance( frontPath, backPath );
       }

       if( hostCellPtr.lock< bool >( [ & ]( Scripting::WallCell& wallCell ) { return isWallDimensionPresent( frontPath, backPath, wallCell.y ); } ) ) {
         newYWallInstance( frontPath, backPath );
       }

       // TODO: D and R segments
     }

    void WallCellBundler::render() {
      if( x ) {
        x->drawEntity();
      }

      if( y ) {
        y->drawEntity();
      }
    }

    bool WallCellBundler::isWallDimensionPresent( std::string& frontPath, std::string& backPath, std::unique_ptr< Scripting::WallCell::Segment >& ptr ) {
      if( ptr ) {
        frontPath.assign( ptr->front.lock< std::string >( [ & ]( Scripting::Wallpaper& wallpaper ) { return wallpaper.imagePath; } ) );
        backPath.assign( ptr->back.lock< std::string >( [ & ]( Scripting::Wallpaper& wallpaper ) { return wallpaper.imagePath; } ) );
        return true;
      } else {
        return false;
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

    void WallCellBundler::newXWallInstance( std::string& frontWallpaper, std::string& backWallpaper ) {
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

      glm::vec3 position( center.x, center.y + 0.9f, center.z );

      // Stay in scope for the material call below
      // THIS IS ALREADY STARTING TO SUCK
      std::shared_ptr< sf::Image > potentialSide;

      if ( currentRotation == 0 || currentRotation == 1 ) {
        position.y = position.y + 0.1f;

        // For the Rotation 0 nudge, there's a couple of things that need to be taken care of on the neighbouring instance
        // TODO: rework
        /*
        if( currentRotation == 0 ) {
          std::shared_ptr< WallCellBundler > top = topNeighbour.lock();

          if( top && top->y ) {
            // This segment needs to be removed as X will provide it
            top->y->children.erase( "RightCorner" );

            // We'll need to retexture Side2 to be the front face of this wallpaper
            std::string upperFront = top->hostCellPtr.lock< std::string >( [ & ]( Scripting::WallCell& wallCell ) {
              return wallCell.y->front.lock< std::string >( [ & ]( Scripting::Wallpaper& wallpaper ) { return wallpaper.imagePath; } );
            } );

            settings.erase( "Side2" );
            potentialSide = getSegmentBundle( upperFront, false, false, true ).rightSegment;
            settings.emplace( std::make_pair( "Side2", std::make_unique< DirectImageSource >( *potentialSide, "0xs2 " + upperFront ) ) );
          }
        }
        */
      }

      x->setPosition( position );
      x->setRotationAngle( glm::radians( 180.0f ) );

      std::shared_ptr< Material > material = std::make_shared< Material >( hostTextureCache.getUsingAtlas( WALLATLAS_PATH, settings ) );

      x->drawable->material = material;
      x->findChildByName( "LeftCorner" )->drawable->material = material;
      x->findChildByName( "RightCorner" )->drawable->material = material;
    }

    void WallCellBundler::newYWallInstance( std::string& frontWallpaper, std::string& backWallpaper ) {
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

      glm::vec3 position( center.x - 0.9f, center.y, center.z );
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
