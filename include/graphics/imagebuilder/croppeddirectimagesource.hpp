#ifndef CROPIMAGESOURCE
#define CROPIMAGESOURCE

#include "graphics/imagebuilder/directimagesource.hpp"
#include <SFML/Graphics.hpp>
#include <string>

namespace BlueBear {
  namespace Graphics {

    class CroppedDirectImageSource : public DirectImageSource {
      unsigned int x;
      unsigned int y;
      unsigned int w;
      unsigned int h;
    public:
      CroppedDirectImageSource( sf::Image& image, unsigned int x, unsigned int y, unsigned int w, unsigned int h, const std::string& key );
      sf::Image getImage();
      std::string getKey();
    };

  }
}

#endif
