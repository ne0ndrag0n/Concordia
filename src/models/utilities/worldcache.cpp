#include "models/utilities/worldcache.hpp"
#include "tools/utility.hpp"
#include "log.hpp"
#include <vector>

namespace BlueBear::Models::Utilities {

  WorldCache::WorldCache() {
    loadFlooring();
    loadWallpaper();
  }

  std::shared_ptr< sf::Image > WorldCache::loadImage( const std::string& path ) {
    auto it = images.find( path );
    if( it != images.end() ) {
      return it->second;
    }

    std::shared_ptr< sf::Image > result = std::make_shared< sf::Image >();
    if( !result->loadFromFile( path ) ) {
      Log::getInstance().error( "WorldCache::loadImage", "Invalid path: " + path );
      throw InvalidImageException();
    }

    images[ path ] = result;
    return result;
  }

  void WorldCache::loadWallpaper() {
    const std::string path = "assets/wallpaper";
    std::vector< std::string > subdirectoryList = Tools::Utility::getSubdirectoryList( path.c_str() );

    for( const std::string& wallpaperPack : subdirectoryList ) {
      std::string packPath = path + "/" + wallpaperPack;
      Json::Value root = Tools::Utility::fileToJson( packPath + "/base.json" );
      for( auto it = root.begin(); it != root.end(); ++it ) {
        auto pair = Tools::Utility::jsonIteratorToPair( it );

        if( originalWallpapers.find( pair.first ) != originalWallpapers.end() ) {
          Log::getInstance().warn( "WorldCache::loadWallpaper", "Duplicate wallpaper type registered and will be overwritten: " + pair.first );
        }

        try {
          originalWallpapers[ pair.first ] = Wallpaper {
            loadImage( packPath + "/" + pair.second.get()[ "image" ].asString() ),
            pair.second.get()[ "price" ].asDouble()
          };

          Log::getInstance().debug( "WorldCache::loadWallpaper", "Registered wallpaper tile type " + pair.first );
        } catch( std::exception e ) {}
      }
    }
  }

  void WorldCache::loadFlooring() {
    const std::string path = "assets/flooring";
    std::vector< std::string > subdirectoryList = Tools::Utility::getSubdirectoryList( path.c_str() );

    for( const std::string& floorpack : subdirectoryList ) {
      std::string packPath = path + "/" + floorpack;
      Json::Value root = Tools::Utility::fileToJson( packPath + "/base.json" );
      for( auto it = root.begin(); it != root.end(); ++it ) {
        auto pair = Tools::Utility::jsonIteratorToPair( it );

        if( originalTiles.find( pair.first ) != originalTiles.end() ) {
          Log::getInstance().warn( "WorldCache::loadFlooring", "Duplicate tile type registered and will be overwritten: " + pair.first );
        }

        try {
          originalTiles[ pair.first ] = FloorTile {
            loadImage( packPath + "/" + pair.second.get()[ "image" ].asString() ),
            pair.second.get()[ "price" ].asDouble()
          };

          Log::getInstance().debug( "WorldCache::loadFlooring", "Registered floor tile type " + pair.first );
        } catch( std::exception e ) {}
      }
    }
  }

  std::optional< FloorTile > WorldCache::getFloorTile( const std::string& id ) {
    auto originalTile = originalTiles.find( id );
    if( originalTile != originalTiles.end() ) {
      return originalTile->second;
    } else {
      return {};
    }
  }

  std::optional< Wallpaper > WorldCache::getWallpaper( const std::string& id ) {
    auto originalWallpaper = originalWallpapers.find( id );
    if( originalWallpaper != originalWallpapers.end() ) {
      return originalWallpaper->second;
    } else {
      return {};
    }
  }



}
