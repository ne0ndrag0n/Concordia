#ifndef DIRECTIMAGESOURCE
#define DIRECTIMAGESOURCE

#include "graphics/imagebuilder/imagesource.hpp"
#include <SFML/Graphics.hpp>
#include <string>

namespace BlueBear {
  namespace Graphics {

    /**
     * The object referred to by imageReference must STAY VALID!!
     * TODO: We change this to std::shared_ptr. This reference is too unsafe.
     */
    class DirectImageSource : public ImageSource {
    protected:
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
