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
      std::shared_ptr< sf::Image > getImage( std::shared_ptr< ImageSource > source );
      void prune();
    };

  }
}


#endif
