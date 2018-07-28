#ifndef WALLPAPER_REGION
#define WALLPAPER_REGION

#include "models/wallpaper.hpp"
#include <optional>

namespace BlueBear::Models {

  struct WallpaperRegion {
    std::optional< Wallpaper > north;
    std::optional< Wallpaper > east;
    std::optional< Wallpaper > west;
    std::optional< Wallpaper > south;
  };

}

#endif
