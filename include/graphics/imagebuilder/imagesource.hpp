#ifndef IMAGEBUILDER
#define IMAGEBUILDER

#include <SFML/Graphics.hpp>
#include <string>

namespace BlueBear {
  namespace Graphics {

    // Abstract !!
    class ImageSource {
      public:
        virtual sf::Image getImage() = 0;
        virtual std::string getKey() = 0;
    };

  }
}


#endif
