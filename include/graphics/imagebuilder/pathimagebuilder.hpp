#ifndef PATHIMAGEBUILDER
#define PATHIMAGEBUILDER

#include "graphics/imagebuilder/imagebuilder.hpp"
#include <SFML/Graphics.hpp>
#include <string>

namespace BlueBear {
  namespace Graphics {

    class PathImageBuilder : public ImageBuilder {
      std::string path;

      public:
        PathImageBuilder( const std::string& path );
        void setPath( const std::string& path );
        std::string getPath();

        sf::Image getImage();
    };

  }
}

#endif
