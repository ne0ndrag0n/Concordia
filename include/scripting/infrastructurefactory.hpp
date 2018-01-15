#ifndef INFRASTRUCTUREFACTORY
#define INFRASTRUCTUREFACTORY

#include "exceptions/genexc.hpp"
#include <string>
#include <jsoncpp/json/json.h>
#include <memory>
#include <map>

namespace BlueBear {
  namespace Models {
    class Tile;
  }

  namespace Scripting {

    class Wallpaper;

    class InfrastructureFactory {
      // Phase in new static convention here
      static constexpr const char* GREY_SYSTEM_WALLPAPER = "system/models/wall/greywallpaper.png";
      static constexpr const char* TILE_SYSTEM_PATH = "system/flooring/";
      static constexpr const char* TILE_ASSETS_PATH = "assets/flooring/";
      static constexpr const char* WALL_ASSETS_PATH = "assets/wallpaper/";
      static constexpr const char* TILE_SYSTEM_ROOT = "base.json";
      static constexpr const char* WALL_SYSTEM_ROOT = "base.json";

      Json::Value tileConstants;
      std::map< std::string, std::shared_ptr< Models::Tile > > tileRegistry;
      std::map< std::string, std::shared_ptr< Wallpaper > > wallpaperRegistry;

      void registerFloorTile( const std::string& path );
      void registerWallpaper( const std::string& path );
      std::string getVariableOrValue( const std::string& key, const std::string& value );

      void registerFloorTiles();
      void registerWallpapers();

      public:
        InfrastructureFactory();
        EXCEPTION_TYPE( CannotLoadFileException, "Could not load a required file!" );

        std::shared_ptr< Models::Tile > getFloorTile( const std::string& key );
        std::shared_ptr< Wallpaper > getWallpaper( const std::string& key );
    };

  }
}


#endif
