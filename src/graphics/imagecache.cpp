#include "graphics/imagecache.hpp"
#include "graphics/imagebuilder/imagesource.hpp"
#include "configmanager.hpp"
#include <SFML/Graphics.hpp>
#include <string>
#include <map>
#include <memory>

namespace BlueBear {
  namespace Graphics {

    std::shared_ptr< sf::Image > ImageCache::getImage( ImageSource& source ) {

      if( ConfigManager::getInstance().getBoolValue( "disable_image_cache" ) == true ) {
        return std::make_shared< sf::Image >( source.getImage() );
      }

      std::string key = source.getKey();

      auto kvPair = imageCache.find( key );
      if( kvPair != imageCache.end() ) {
        return kvPair->second;
      }

      return imageCache[ key ] = std::make_shared< sf::Image >( source.getImage() );
    }

    void ImageCache::prune() {
      auto iterator = std::begin( imageCache );

      while( iterator != std::end( imageCache ) ) {
        if( iterator->second.unique() ) {
          iterator = imageCache.erase( iterator );
        } else {
          iterator++;
        }
      }
    }

  }
}
