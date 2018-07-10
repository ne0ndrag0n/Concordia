#ifndef WORLDCACHE
#define WORLDCACHE

#include "exceptions/genexc.hpp"
#include "models/floortile.hpp"
#include <SFML/Graphics/Image.hpp>
#include <optional>
#include <string>
#include <memory>
#include <map>

namespace BlueBear::Models::Utilities {

  class WorldCache {
    std::map< std::string, FloorTile > originalTiles;
    std::map< std::string, std::shared_ptr< sf::Image > > images;

    std::shared_ptr< sf::Image > loadImage( const std::string& path );
    void loadFlooring();

  public:
    EXCEPTION_TYPE( InvalidImageException, "Invalid image path!" );

    WorldCache();

    std::optional< FloorTile > getFloorTile( const std::string& id );
  };

}

#endif
