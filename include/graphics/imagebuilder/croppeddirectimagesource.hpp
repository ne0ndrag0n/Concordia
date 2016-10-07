#ifndef CROPIMAGESOURCE
#define CROPIMAGESOURCE

#include "graphics/imagebuilder/directimagesource.hpp"
#include <SFML/Graphics.hpp>
#include <string>

namespace BlueBear {
  namespace Graphics {

    class CroppedDirectImageSource : public DirectImageSource {
      int x;
      int y;
      int w;
      int h;
    public:
      CroppedDirectImageSource( sf::Image& image, int x, int y, int w, int h, const std::string& key );
      sf::Image getImage();
      std::string getKey();
    };

  }
}

#endif
