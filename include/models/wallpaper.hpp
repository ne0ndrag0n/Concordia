#ifndef WALLPAPER
#define WALLPAPER

#include <SFGUI/Image.hpp>
#include <memory>

namespace BlueBear::Models {

  struct Wallpaper {
    std::shared_ptr< sf::Image > surface;
    double price = 0.0;
  };

}

#endif
