#ifndef IMAGECACHE
#define IMAGECACHE

#include <SFML/Graphics.hpp>
#include <string>
#include <map>
#include <memory>

namespace BlueBear {
  namespace Graphics {
    class ImageSource;

    class ImageCache {
      std::map< std::string, std::shared_ptr< sf::Image > > imageCache;

    public:
      // References are polymorphic so we are migrating to this across the board (get rid of that damn shared_ptr habit ffs).
      std::shared_ptr< sf::Image > getImage( ImageSource& source );
      void prune();
    };

  }
}


#endif
