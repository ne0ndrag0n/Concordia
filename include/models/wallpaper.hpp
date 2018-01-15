#ifndef WALLPAPER
#define WALLPAPER

#include <string>

namespace BlueBear {
  namespace Models {

    struct Wallpaper {
      std::string id;
      std::string imagePath;
      double wallpaperPrice;

      Wallpaper( const std::string& id, const std::string& imagePath, double wallpaperPrice ) : id( id ), imagePath( imagePath ), wallpaperPrice( wallpaperPrice ) {}
    };

  }
}

#endif
