#ifndef DIRECTIMAGESOURCE
#define DIRECTIMAGESOURCE

#include "graphics/imagebuilder/imagesource.hpp"
#include <SFML/Graphics.hpp>
#include <string>

namespace BlueBear {
  namespace Graphics {

    /**
     * The object referred to by imageReference must STAY VALID!!
     */
    class DirectImageSource : public ImageSource {
      sf::Image& imageReference;
      std::string key;

    public:
      DirectImageSource( sf::Image& image, const std::string& key );
      sf::Image getImage();
      std::string getKey();
    };

  }
}


#endif
