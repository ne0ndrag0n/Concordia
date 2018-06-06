#include "models/infrastructure.hpp"
#include "tools/utility.hpp"
#include <vector>

#include "log.hpp"

namespace BlueBear::Models {

  Infrastructure::Infrastructure( Device::Display::Adapter::Component::WorldRenderer& worldRenderer ) : worldRenderer( worldRenderer ) {
    const std::string path = "assets/flooring";

    std::vector< std::string > subdirectoryList = Tools::Utility::getSubdirectoryList( path.c_str() );

    for( const std::string& floorpack : subdirectoryList ) {
      std::string packPath = path + "/" + floorpack;
      Json::Value root = Tools::Utility::fileToJson( packPath + "/base.json" );
      for( auto it = root.begin(); it != root.end(); ++it ) {
        auto pair = Tools::Utility::jsonIteratorToPair( it );

        if( originalTiles.find( pair.first ) != originalTiles.end() ) {
          Log::getInstance().warn( "Infrastructure::Infrastructure", "Duplicate tile type registered and will be overwritten: " + pair.first );
        }

        try {
          originalTiles[ pair.first ] = FloorTile {
            loadImage( packPath + "/" + pair.second.get()[ "image" ].asString() ),
            pair.second.get()[ "price" ].asDouble()
          };

          Log::getInstance().debug( "Infrastructure::Infrastructure", "Registered floor tile type " + pair.first );
        } catch( std::exception e ) {}
      }
    }
  }

  std::shared_ptr< sf::Image > Infrastructure::loadImage( const std::string& path ) {
    auto it = images.find( path );
    if( it != images.end() ) {
      return it->second;
    }

    std::shared_ptr< sf::Image > result = std::make_shared< sf::Image >();
    if( !result->loadFromFile( path ) ) {
      Log::getInstance().error( "Infrastructure::loadImage", "Invalid path: " + path );
      throw InvalidImageException();
    }

    images[ path ] = result;
    return result;
  }

  Json::Value Infrastructure::save() {

  }

  void Infrastructure::load( const Json::Value& data ) {

  }
}
