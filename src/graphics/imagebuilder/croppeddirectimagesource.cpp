#include "graphics/imagebuilder/croppeddirectimagesource.hpp"
#include "graphics/imagebuilder/directimagesource.hpp"
#include <SFML/Graphics.hpp>
#include <string>

namespace BlueBear {
  namespace Graphics {
    CroppedDirectImageSource::CroppedDirectImageSource( sf::Image& image, int x, int y, int w, int h, const std::string& key ) :
      DirectImageSource::DirectImageSource( image, key ), x( x ), y( y ), w( w ), h( h ) {}

    sf::Image CroppedDirectImageSource::getImage() {
      sf::Image parentResult = DirectImageSource::getImage();
      sf::Image result;

      result.create( w, h );
      result.copy( parentResult, 0, 0, { x, y, w, h } );

      return result;
    }

    std::string CroppedDirectImageSource::getKey() {
      std::string key = DirectImageSource::getKey();

      return "c/" +
        std::to_string( x ) + "," +
        std::to_string( y ) + "," +
        std::to_string( w ) + "," +
        std::to_string( h ) + " " +
        key;
    }
  }
}
