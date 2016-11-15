#include "graphics/imagebuilder/pointerimagesource.hpp"
#include <SFML/Graphics.hpp>
#include <string>
#include <memory>

namespace BlueBear {
  namespace Graphics {

    PointerImageSource::PointerImageSource( std::shared_ptr< sf::Image > image, const std::string& key ) :
      image( image ), key( key ) {}

    sf::Image PointerImageSource::getImage() {
      return *image;
    }

    std::string PointerImageSource::getKey() {
      return key;
    }
  }
}
