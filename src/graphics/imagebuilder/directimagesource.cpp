#include "graphics/imagebuilder/directimagesource.hpp"
#include <SFML/Graphics.hpp>
#include <string>

namespace BlueBear {
  namespace Graphics {

    DirectImageSource::DirectImageSource( sf::Image& image, const std::string& key ) : imageReference( image ), key( key ) {}

    /**
     * Copy the reference image to fit the interface.
     */
    sf::Image DirectImageSource::getImage() {
      return imageReference;
    }

    std::string DirectImageSource::getKey() {
      return key;
    }

  }
}
