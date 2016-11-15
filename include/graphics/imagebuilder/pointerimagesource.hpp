#ifndef POINTERIMAGESOURCE
#define POINTERIMAGESOURCE

#include "graphics/imagebuilder/imagesource.hpp"
#include <SFML/Graphics.hpp>
#include <string>
#include <memory>

namespace BlueBear {
  namespace Graphics {

    /**
     * The object referred to by imageReference must STAY VALID!!
     * TODO: We change this to std::shared_ptr. This reference is too unsafe.
     */
    class PointerImageSource : public ImageSource {
    protected:
      std::shared_ptr< sf::Image > image;
      std::string key;

    public:
      PointerImageSource( std::shared_ptr< sf::Image > image, const std::string& key );
      sf::Image getImage();
      std::string getKey();
    };

  }
}


#endif
