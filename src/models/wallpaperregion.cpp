#include "models/wallpaperregion.hpp"
#include "log.hpp"

namespace BlueBear::Models {

  WallpaperRegion::WallpaperRegion( const Json::Value& wallpaperRegion, Utilities::WorldCache& worldCache ) {
    if( wallpaperRegion[ "north" ] != Json::Value::null && wallpaperRegion[ "south" ] != Json::Value::null ) {
      const std::string& northTitle = wallpaperRegion[ "north" ].asString();
      const std::string& southTitle = wallpaperRegion[ "south" ].asString();
      std::optional< Wallpaper > northWallpaper = worldCache.getWallpaper( northTitle );
      std::optional< Wallpaper > southWallpaper = worldCache.getWallpaper( southTitle );

      if( !northWallpaper ) {
        Log::getInstance().warn( "WallpaperRegion::WallpaperRegion", "Missing north wallpaper: " + northTitle );
      } else if( !southWallpaper ) {
        Log::getInstance().warn( "WallpaperRegion::WallpaperRegion", "Missing south wallpaper: " + southTitle );
      } else {
        x.north = { northTitle, *northWallpaper };
        x.south = { southTitle, *southWallpaper };
      }
    }

    if( wallpaperRegion[ "east" ] != Json::Value::null && wallpaperRegion[ "west" ] != Json::Value::null ) {
      const std::string& eastTitle = wallpaperRegion[ "east" ].asString();
      const std::string& westTitle = wallpaperRegion[ "west" ].asString();
      std::optional< Wallpaper > eastWallpaper = worldCache.getWallpaper( eastTitle );
      std::optional< Wallpaper > westWallpaper = worldCache.getWallpaper( westTitle );

      if( !eastWallpaper ) {
        Log::getInstance().warn( "WallpaperRegion::WallpaperRegion", "Missing east wallpaper: " + eastTitle );
      } else if( !westWallpaper ) {
        Log::getInstance().warn( "WallpaperRegion::WallpaperRegion", "Missing west wallpaper: " + westTitle );
      } else {
        y.east = { eastTitle, *eastWallpaper };
        y.west = { westTitle, *westWallpaper };
      }
    }
  }

  bool WallpaperRegion::isEmpty() const {
    return false;
  }

  bool WallpaperRegion::isCorner() const {
    return false;
  }

}
