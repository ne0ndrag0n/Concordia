#include "graphics/imagebuilder/pathimagebuilder.hpp"
#include "exceptions/cannotloadfile.hpp"
#include "log.hpp"
#include <SFML/Graphics.hpp>
#include <string>

namespace BlueBear {
  namespace Graphics {

    PathImageBuilder::PathImageBuilder( const std::string& path ) : path( path ) {}

    void PathImageBuilder::setPath( const std::string& path ) {
      this->path = path;
    }

    sf::Image PathImageBuilder::getImage() {
      sf::Image image;

      if( !image.loadFromFile( path ) ) {
        Log::getInstance().error( "PathImageBuilder::getImage", "Error loading file: " + path );
        throw Exceptions::CannotLoadFileException();
      }

      return image;
    }

  }
}
