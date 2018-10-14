#ifndef WALLPAPER_REGION
#define WALLPAPER_REGION

#include "models/utilities/worldcache.hpp"
#include "models/wallpaper.hpp"
#include <jsoncpp/json/json.h>
#include <utility>
#include <string>
#include <optional>

namespace BlueBear::Models {

  struct WallpaperRegion {
    struct {
      std::optional< std::pair< std::string, Wallpaper > > north;
      std::optional< std::pair< std::string, Wallpaper > > south;
    } x;

    struct {
      std::optional< std::pair< std::string, Wallpaper > > east;
      std::optional< std::pair< std::string, Wallpaper > > west;
    } y;

    WallpaperRegion() = default;
    WallpaperRegion( const Json::Value& wallpaperRegion, Utilities::WorldCache& worldCache );

    bool isEmpty() const;
    bool isCorner() const;
    bool isX() const;
    bool isY() const;
  };

}

#endif
