#include "graphics/wallcellbundler.hpp"
#include "graphics/model.hpp"
#include "graphics/imagecache.hpp"
#include "graphics/texturecache.hpp"
#include "graphics/instance/instance.hpp"
#include "graphics/imagebuilder/imagesource.hpp"
#include "graphics/imagebuilder/pathimagesource.hpp"
#include "graphics/imagebuilder/croppeddirectimagesource.hpp"
#include "graphics/imagebuilder/pointerimagesource.hpp"
#include "graphics/material.hpp"
#include "containers/collection3d.hpp"
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

    float WallCellBundler::xOrigin = 0.0f;
    float WallCellBundler::yOrigin = 0.0f;

    WallCellBundler::WallCellBundler( Threading::Lockable< Scripting::WallCell > hostCell, Containers::Collection3D< std::shared_ptr< WallCellBundler > >& hostCollection, glm::vec3 counter, unsigned int currentRotation, TextureCache& hostTextureCache, ImageCache& hostImageCache, unsigned int shader ) :
     currentRotation( currentRotation ),
     hostTextureCache( hostTextureCache ),
     hostImageCache( hostImageCache ),
     shader( shader ),
     hostCellPtr( hostCell ),
     counter( counter ) {

       center = glm::vec3( xOrigin + counter.x, yOrigin - counter.y, counter.z * 2.0f );

       std::string frontPath;
       std::string backPath;

       if( hostCellPtr.lock< bool >( [ & ]( Scripting::WallCell& wallCell ) { return isWallDimensionPresent( frontPath, backPath, wallCell.x ); } ) ) {
         newXWallInstance( hostCollection, frontPath, backPath );
       }

       if( hostCellPtr.lock< bool >( [ & ]( Scripting::WallCell& wallCell ) { return isWallDimensionPresent( frontPath, backPath, wallCell.y ); } ) ) {
         newYWallInstance( hostCollection, frontPath, backPath );
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

    std::shared_ptr< WallCellBundler > WallCellBundler::safeGetBundler( Containers::Collection3D< std::shared_ptr< WallCellBundler > >& hostCollection, int x, int y, int z ) {
      std::shared_ptr< WallCellBundler > result( nullptr );

      if( x >= 0 && y >= 0 && z >= 0 ) {
        result = hostCollection.getItem( z, x, y );
      }

      return result;
    }

    void WallCellBundler::newXWallInstance( Containers::Collection3D< std::shared_ptr< WallCellBundler > >& hostCollection, std::string& frontWallpaper, std::string& backWallpaper ) {
      x = std::make_unique< Instance >( *WallCellBundler::Piece, shader );

      glm::vec3 position( center.x, center.y + 0.9f, center.z );

      std::map< std::string, std::unique_ptr< ImageSource > > settings;
      SegmentBundle front = getSegmentBundle( frontWallpaper );
      SegmentBundle back = getSegmentBundle( backWallpaper );

      switch( currentRotation ) {
        case 0:
          {
            // This rotation requires a nudge
            position.y = position.y + 0.1f;

            settings.emplace( std::make_pair( "BackWallLeft", std::make_unique< PointerImageSource >( back.leftSegment, "0xl " + backWallpaper ) ) );
            settings.emplace( std::make_pair( "BackWallCenter", std::make_unique< PointerImageSource >( back.centerSegment, "0xc " + backWallpaper ) ) );
            settings.emplace( std::make_pair( "BackWallRight", std::make_unique< PointerImageSource >( back.rightSegment, "0xr " + backWallpaper ) ) );

            // Now let's determine what Side2 should be on this nudged piece

            // CASE: Newly placed X-Segment collides with Y-Segment in cell above.
            std::shared_ptr< WallCellBundler > top = safeGetBundler( hostCollection, counter.x, counter.y - 1, counter.z );
            bool topContainsY = top && top->y;
            if( topContainsY ) {
              // This nudge will result in a collision with the Y-segment piece in the cell above
              // Handle this by deleting the Y-segment piece's RightCorner segment, then setting Side2 of this new model to its front image.

              // Delete the Y-segment piece's RightCorner segment so nothing can collide
              // We can either collide with the top RightCorner, or if the piece to the left was directly before a lower right corner, the left X-segment's ExtendedSegment
              if( top->y->children.find( "RightCorner" ) != top->y->children.end() ) {
                top->y->children.erase( "RightCorner" );
              } else {
                // The only way this would happen is if RightCorner was already deleted in favor of X to the left's ExtendedSegment. That'll have to go now.
                std::shared_ptr< WallCellBundler > left = safeGetBundler( hostCollection, counter.x - 1, counter.y, counter.z );
                left->x->children.erase( "ExtendedSegment" );
              }

              // Get "upperFront", which is the front image path for the Y-segment wall
              std::string upperFront = top->hostCellPtr.lock< std::string >( [ & ]( Scripting::WallCell& wallCell ) {
                return wallCell.y->front.lock< std::string >( [ & ]( Scripting::Wallpaper& wallpaper ) { return wallpaper.imagePath; } );
              } );

              // Using upperFront, emplace Side2 as the rightSegment image pointer for that path
              settings.emplace( std::make_pair( "Side2", std::make_unique< PointerImageSource >( getSegmentBundle( upperFront, false, false, true ).rightSegment, "0xs2 " + upperFront ) ) );
            } else {
              // This nudge will not result in any collision with the cell above (or there is no actual cell above). Let's go with the usual plan for Side2.
              settings.emplace( std::make_pair( "Side2", std::make_unique< PointerImageSource >( back.rightSegment, "0xs2 " + backWallpaper ) ) );
            }

            // CASE: The placed X-segment causes an inconsistent corner due to the presence of a Y-segment at x + 1, y - 1 (upper right corner relative to this cell)
            // What needs to be done here: Remove the RightCorner in the upper right corner and create an ExtendedSegment in this X piece
            std::shared_ptr< WallCellBundler > upperRight = safeGetBundler( hostCollection, counter.x + 1, counter.y - 1, counter.z );
            bool upperRightContainsY = upperRight && upperRight->y;
            if( upperRightContainsY ) {
              // Copy X-segment's RightCorner and move it
              upperRight->y->children.erase( "RightCorner" );

              std::shared_ptr< Instance > xExtended = std::make_shared< Instance >( *( x->children.at( "RightCorner" ) ) );
              glm::vec3 position = xExtended->getPosition();
              position.x = position.x - 1.0f;
              xExtended->setPosition( position );
              x->children[ "ExtendedSegment" ] = xExtended;
            }
          }
          break;
        case 1:
          {
            // This rotation requires a nudge
            position.y = position.y + 0.1f;

            settings.emplace( std::make_pair( "BackWallLeft", std::make_unique< PointerImageSource >( back.leftSegment, "1xl " + backWallpaper ) ) );
            settings.emplace( std::make_pair( "BackWallCenter", std::make_unique< PointerImageSource >( back.centerSegment, "1xc " + backWallpaper ) ) );
            settings.emplace( std::make_pair( "BackWallRight", std::make_unique< PointerImageSource >( back.rightSegment, "1xr " + backWallpaper ) ) );

            // CASE: X-segment collides with upper-right cell, which may have nudged a Y segment into it
            std::shared_ptr< WallCellBundler > upperRight = safeGetBundler( hostCollection, counter.x + 1, counter.y - 1, counter.z );
            bool upperRightContainsY = upperRight && upperRight->y;
            if( upperRightContainsY ) {
              upperRight->y->children.erase( "RightCorner" );

              std::string upperRightBack = upperRight->hostCellPtr.lock< std::string >( [ & ]( Scripting::WallCell& wallCell ) {
                return wallCell.y->back.lock< std::string >( [ & ]( Scripting::Wallpaper& wallpaper ) { return wallpaper.imagePath; } );
              } );

              settings.emplace( std::make_pair( "Side1", std::make_unique< PointerImageSource >( getSegmentBundle( upperRightBack, true, false, false ).leftSegment, "1xs1 " + upperRightBack ) ) );
            } else {
              settings.emplace( std::make_pair( "Side1", std::make_unique< PointerImageSource >( back.leftSegment, "1xs1 " + backWallpaper ) ) );
            }

            // CASE: X-segment creates incomplete lower-left corner in a box-shaped wall
            std::shared_ptr< WallCellBundler > top = safeGetBundler( hostCollection, counter.x, counter.y - 1, counter.z );
            std::shared_ptr< WallCellBundler > left = safeGetBundler( hostCollection, counter.x - 1, counter.y, counter.z );
            bool topContainsY = top && top->y;
            bool leftContainsX = left && left->x;
            if( topContainsY && !leftContainsX ) {
              top->y->children.erase( "RightCorner" );

              std::shared_ptr< Instance > xExtended = std::make_shared< Instance >( *( x->children.at( "LeftCorner" ) ) );
              glm::vec3 position = xExtended->getPosition();
              position.x = position.x + 1.0f;
              xExtended->setPosition( position );
              x->children[ "ExtendedSegment" ] = xExtended;
            }
          }
          break;
        case 2:
          {
            settings.emplace( std::make_pair( "FrontWallLeft", std::make_unique< PointerImageSource >( front.leftSegment, "2xl " + frontWallpaper ) ) );
            settings.emplace( std::make_pair( "FrontWallCenter", std::make_unique< PointerImageSource >( front.centerSegment, "2xc " + frontWallpaper ) ) );
            settings.emplace( std::make_pair( "FrontWallRight", std::make_unique< PointerImageSource >( front.rightSegment, "2xr " + frontWallpaper ) ) );

            // CASE: Open corner to the left of this tile due to a Y-segment directly above
            std::shared_ptr< WallCellBundler > top = safeGetBundler( hostCollection, counter.x, counter.y - 1, counter.z );
            bool topContainsY = top && top->y;
            if( topContainsY ) {
              std::shared_ptr< Instance > xExtended = std::make_shared< Instance >( *( x->children.at( "RightCorner" ) ) );
              glm::vec3 position = xExtended->getPosition();
              // test
              position.x = position.x + 0.1f;
              xExtended->setPosition( position );
              x->children[ "ExtendedSegment" ] = xExtended;
            }

            // CASE: Placement of X-segment causes incomplete corner with upper-right Y segment
            std::shared_ptr< WallCellBundler > upperRight = safeGetBundler( hostCollection, counter.x + 1, counter.y - 1, counter.z );
            bool upperRightContainsY = upperRight && upperRight->y;
            if( upperRightContainsY ) {
              // All we have to do is retexture Side1!
              std::string back = upperRight->hostCellPtr.lock< std::string >( [ & ]( Scripting::WallCell& wallCell ) {
                return wallCell.y->back.lock< std::string >( [ & ]( Scripting::Wallpaper& wallpaper ) { return wallpaper.imagePath; } );
              } );

              settings.emplace( std::make_pair( "Side1", std::make_unique< PointerImageSource >( getSegmentBundle( back, false, false, true ).rightSegment, "2xs1 " + back ) ) );
            } else {
              settings.emplace( std::make_pair( "Side1", std::make_unique< PointerImageSource >( front.rightSegment, "2xs1 " + frontWallpaper ) ) );
            }
          }
          break;
        case 3:
        default:
          {
            settings.emplace( std::make_pair( "FrontWallLeft", std::make_unique< PointerImageSource >( front.leftSegment, "3xl " + frontWallpaper ) ) );
            settings.emplace( std::make_pair( "FrontWallCenter", std::make_unique< PointerImageSource >( front.centerSegment, "3xc " + frontWallpaper ) ) );
            settings.emplace( std::make_pair( "FrontWallRight", std::make_unique< PointerImageSource >( front.rightSegment, "3xr " + frontWallpaper ) ) );

            // CASE: X-segment we're about to place may collide with an ExtendedSegment from the left
            std::shared_ptr< WallCellBundler > left = safeGetBundler( hostCollection, counter.x - 1, counter.y, counter.z );
            bool leftContainsX = left && left->x;
            if( leftContainsX ) {
              if( left->x->children.find( "ExtendedSegment" ) != left->x->children.end() ) {
                left->x->children.erase( "ExtendedSegment" );
              }
            }

            // CASE: X-segment we're about to place causes an open gap because there is a Y-segment in the upper right cell
            std::shared_ptr< WallCellBundler > upperRight = safeGetBundler( hostCollection, counter.x + 1, counter.y - 1, counter.z );
            bool upperRightContainsY = upperRight && upperRight->y;
            if( upperRightContainsY ) {
              std::shared_ptr< Instance > xExtended = std::make_shared< Instance >( *( x->children.at( "LeftCorner" ) ) );
              glm::vec3 position = xExtended->getPosition();
              position.x = position.x - 0.1f;
              xExtended->setPosition( position );
              x->children[ "ExtendedSegment" ] = xExtended;
            }

            std::shared_ptr< WallCellBundler > top = safeGetBundler( hostCollection, counter.x, counter.y - 1, counter.z );
            bool topContainsY = top && top->y;
            if( topContainsY && !leftContainsX ) {
              // CASE: Placing an X in this cell, there is a Y on top, and no X in the left cell. An incomplete corner occurs.

              // Need to get front wallpaper for Y panel on top and apply it to Side2
              std::string frontWallpaper = top->hostCellPtr.lock< std::string >( [ & ]( Scripting::WallCell& wallCell ) {
                return wallCell.y->front.lock< std::string >( [ & ]( Scripting::Wallpaper& wallpaper ) { return wallpaper.imagePath; } );
              } );

              settings.emplace( std::make_pair( "Side2", std::make_unique< PointerImageSource >( getSegmentBundle( frontWallpaper, true, false, true ).leftSegment, "3xs2 " + frontWallpaper ) ) );
            } else {
              settings.emplace( std::make_pair( "Side2", std::make_unique< PointerImageSource >( front.leftSegment, "3xs2 " + frontWallpaper ) ) );
            }
          }
      }

      x->setPosition( position );
      x->setRotationAngle( glm::radians( 180.0f ) );

      std::shared_ptr< Material > material = std::make_shared< Material >( hostTextureCache.getUsingAtlas( WALLATLAS_PATH, settings ) );

      x->drawable->material = material;
      x->findChildByName( "LeftCorner" )->drawable->material = material;
      x->findChildByName( "RightCorner" )->drawable->material = material;
    }

    void WallCellBundler::newYWallInstance( Containers::Collection3D< std::shared_ptr< WallCellBundler > >& hostCollection, std::string& frontWallpaper, std::string& backWallpaper ) {
      y = std::make_unique< Instance >( *WallCellBundler::Piece, shader );

      glm::vec3 position( center.x - 0.9f, center.y, center.z );

      std::map< std::string, std::unique_ptr< ImageSource > > settings;
      SegmentBundle front = getSegmentBundle( frontWallpaper );
      SegmentBundle back = getSegmentBundle( backWallpaper );

      switch( currentRotation ) {
        case 0:
          {
            settings.emplace( std::make_pair( "FrontWallLeft", std::make_unique< PointerImageSource >( front.leftSegment, "0yl " + frontWallpaper ) ) );
            settings.emplace( std::make_pair( "FrontWallCenter", std::make_unique< PointerImageSource >( front.centerSegment, "0yc " + frontWallpaper ) ) );
            settings.emplace( std::make_pair( "FrontWallRight", std::make_unique< PointerImageSource >( front.rightSegment, "0yr " + frontWallpaper ) ) );
            settings.emplace( std::make_pair( "Side2", std::make_unique< PointerImageSource >( front.leftSegment, "0ys2 " + frontWallpaper ) ) );

            // CASE: There is an X-segment in the same cell, and this Y-segment will need a replacement piece to make sure the entire side of the wall is displayed.
            // There is no Y-piece in the cell above, which would negate the need for this.
            std::shared_ptr< WallCellBundler > top = safeGetBundler( hostCollection, counter.x, counter.y - 1, counter.z );
            std::shared_ptr< WallCellBundler > left = safeGetBundler( hostCollection, counter.x - 1, counter.y, counter.z );

            bool topContainsY = top && top->y;
            bool currentContainsX = x.operator bool();
            bool leftContainsX = left && left->x;

            if( !topContainsY && ( currentContainsX != leftContainsX ) ) {

              if( currentContainsX ) {
                x->children.erase( "RightCorner" );
              }

              // Copy Y-segment's RightCorner into a new pointer
              // Drawable is not copied!!
              std::shared_ptr< Instance > yExtended = std::make_shared< Instance >( *( y->children.at( "RightCorner" ) ) );
              glm::vec3 position = yExtended->getPosition();
              position.x = position.x - 1.0f;
              yExtended->setPosition( position );
              y->children[ "ExtendedSegment" ] = yExtended;
            }
          }
          break;
        case 1:
          {
            position.x = position.x - 0.1f;

            settings.emplace( std::make_pair( "BackWallLeft", std::make_unique< PointerImageSource >( back.leftSegment, "1yl " + backWallpaper ) ) );
            settings.emplace( std::make_pair( "BackWallCenter", std::make_unique< PointerImageSource >( back.centerSegment, "1yc " + backWallpaper ) ) );
            settings.emplace( std::make_pair( "BackWallRight", std::make_unique< PointerImageSource >( back.rightSegment, "1yr " + backWallpaper ) ) );
            settings.emplace( std::make_pair( "Side2", std::make_unique< PointerImageSource >( back.rightSegment, "1ys2 " + backWallpaper ) ) );

            // CASE: There is an incomplete corner for wall boxes formed at their upper right corners
            std::shared_ptr< WallCellBundler > top = safeGetBundler( hostCollection, counter.x, counter.y - 1, counter.z );
            std::shared_ptr< WallCellBundler > left = safeGetBundler( hostCollection, counter.x - 1, counter.y, counter.z );

            bool currentContainsX = x.operator bool();
            bool leftContainsX = left && left->x;
            bool topContainsY = top && top->y;

            if( !currentContainsX && leftContainsX && !topContainsY ) {
              left->x->children.erase( "LeftCorner" );

              std::shared_ptr< Instance > yExtended = std::make_shared< Instance >( *( y->children.at( "RightCorner" ) ) );
              glm::vec3 position = yExtended->getPosition();
              position.x = position.x - 1.0f;
              yExtended->setPosition( position );
              y->children[ "ExtendedSegment" ] = yExtended;
            }

            // CASE: Gap corner when this cell has an X, top cell has no Y, and left cell has no X
            if( currentContainsX && !topContainsY && !leftContainsX ) {
              std::shared_ptr< Instance > yExtended = std::make_shared< Instance >( *( y->children.at( "RightCorner" ) ) );
              glm::vec3 position = yExtended->getPosition();
              position.x = position.x - 1.0f;
              yExtended->setPosition( position );
              y->children[ "ExtendedSegment" ] = yExtended;
            }
          }
          break;
        case 2:
          {
            position.x = position.x - 0.1f;

            settings.emplace( std::make_pair( "BackWallLeft", std::make_unique< PointerImageSource >( back.leftSegment, "2yl " + backWallpaper ) ) );
            settings.emplace( std::make_pair( "BackWallCenter", std::make_unique< PointerImageSource >( back.centerSegment, "2yc " + backWallpaper ) ) );
            settings.emplace( std::make_pair( "BackWallRight", std::make_unique< PointerImageSource >( back.rightSegment, "2yr " + backWallpaper ) ) );

            // FIXME this fucking mess

            // CASE: Placing this Y will collide with a piece designed to fill a corner gap
            bool currentContainsX = x.operator bool();
            if( currentContainsX && x->children.find( "ExtendedSegment" ) != x->children.end() ) {
              x->children.erase( "ExtendedSegment" );
            }

            // CASE: Y-segment collides with X-segment to the left
            std::shared_ptr< WallCellBundler > left = safeGetBundler( hostCollection, counter.x - 1, counter.y, counter.z );
            bool leftContainsX = left && left->x;
            if( leftContainsX ) {
              left->x->children.erase( "LeftCorner" );

              std::string leftFront = left->hostCellPtr.lock< std::string >( [ & ]( Scripting::WallCell& wallCell ) {
                return wallCell.x->front.lock< std::string >( [ & ]( Scripting::Wallpaper& wallpaper ) { return wallpaper.imagePath; } );
              } );

              settings.emplace( std::make_pair( "Side1", std::make_unique< PointerImageSource >( getSegmentBundle( leftFront, true, false, false ).leftSegment, "2ys1 " + leftFront ) ) );
            } else {
              // CASE: If no X segment is to the left, but there is an X segment in the current cell, this forms an incomplete corner.
              if( currentContainsX ) {
                std::string xFront = hostCellPtr.lock< std::string >( [ & ]( Scripting::WallCell& wallCell ) {
                  return wallCell.x->front.lock< std::string >( [ & ]( Scripting::Wallpaper& wallpaper ) { return wallpaper.imagePath; } );
                } );

                settings.emplace( std::make_pair( "Side1", std::make_unique< PointerImageSource >( getSegmentBundle( xFront, true, false, false ).leftSegment, "2ys1 " + xFront ) ) );
              } else {
                settings.emplace( std::make_pair( "Side1", std::make_unique< PointerImageSource >( back.leftSegment, "2ys1 " + backWallpaper ) ) );
              }
            }
          }
          break;
        case 3:
        default:
          {
            settings.emplace( std::make_pair( "FrontWallLeft", std::make_unique< PointerImageSource >( front.leftSegment, "3yl " + frontWallpaper ) ) );
            settings.emplace( std::make_pair( "FrontWallCenter", std::make_unique< PointerImageSource >( front.centerSegment, "3yc " + frontWallpaper ) ) );
            settings.emplace( std::make_pair( "FrontWallRight", std::make_unique< PointerImageSource >( front.rightSegment, "3yr " + frontWallpaper ) ) );
            settings.emplace( std::make_pair( "Side1", std::make_unique< PointerImageSource >( front.rightSegment, "3ys1 " + frontWallpaper ) ) );

            // CASE: Y-segment we're about to place may collide with an ExtendedSegment from the left
            std::shared_ptr< WallCellBundler > left = safeGetBundler( hostCollection, counter.x - 1, counter.y, counter.z );
            bool leftContainsX = left && left->x;
            if( leftContainsX ) {
              if( left->x->children.find( "ExtendedSegment" ) != left->x->children.end() ) {
                left->x->children.erase( "ExtendedSegment" );
              }
            }
          }
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
