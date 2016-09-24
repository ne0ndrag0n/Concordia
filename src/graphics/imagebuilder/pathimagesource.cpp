#include "graphics/imagebuilder/pathimagesource.hpp"
#include "exceptions/cannotloadfile.hpp"
#include "log.hpp"
#include <SFML/Graphics.hpp>
#include <string>

namespace BlueBear {
  namespace Graphics {

    PathImageSource::PathImageSource( const std::string& path ) : path( path ) {}

    void PathImageSource::setPath( const std::string& path ) {
      this->path = path;
    }

    sf::Image PathImageSource::getImage() {
      sf::Image image;

      if( !image.loadFromFile( path ) ) {
        Log::getInstance().error( "PathImageSource::getImage", "Error loading file: " + path );
        throw Exceptions::CannotLoadFileException();
      }

      return image;
    }

    /**
     * Get a string key that represents an object of this configuration (used for storage in maps)
     */
    std::string PathImageSource::getKey() {
      return path;
    }

  }
}
