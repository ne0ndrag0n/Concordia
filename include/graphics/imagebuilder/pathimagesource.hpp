#ifndef PATHIMAGEBUILDER
#define PATHIMAGEBUILDER

#include "graphics/imagebuilder/imagesource.hpp"
#include <SFML/Graphics.hpp>
#include <string>

namespace BlueBear {
  namespace Graphics {

    class PathImageSource : public ImageSource {
      std::string path;

      public:
        PathImageSource( const std::string& path );
        void setPath( const std::string& path );
        std::string getPath();

        sf::Image getImage();
        std::string getKey();
    };

  }
}

#endif
