#ifndef WALLPAPER_REGION
#define WALLPAPER_REGION

#include "models/wallpaper.hpp"
#include <utility>
#include <string>
#include <optional>

namespace BlueBear::Models {

  struct WallpaperRegion {
    std::pair< std::string, std::optional< Wallpaper > > north;
    std::pair< std::string, std::optional< Wallpaper > > east;
    std::pair< std::string, std::optional< Wallpaper > > west;
    std::pair< std::string, std::optional< Wallpaper > > south;
  };

}

#endif
