#ifndef IMAGEBUILDER
#define IMAGEBUILDER

#include <SFML/Graphics.hpp>

namespace BlueBear {
  namespace Graphics {

    // Abstract !!
    class ImageBuilder {
      public:
        virtual sf::Image getImage() = 0;
    };

  }
}


#endif
