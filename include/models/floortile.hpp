#ifndef FLOORTILE
#define FLOORTILE

#include <memory>
#include <SFML/Graphics/Image.hpp>

namespace BlueBear::Models {

  struct FloorTile {
    // TODO: footstep sound pointer
    std::shared_ptr< sf::Image > surface;
    double price = 0.0;
  };

}

#endif
