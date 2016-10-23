#ifndef INFRASTRUCTUREFACTORY
#define INFRASTRUCTUREFACTORY

#include "threading/lockable.hpp"
#include <string>
#include <exception>
#include <jsoncpp/json/json.h>
#include <memory>
#include <map>

namespace BlueBear {
  namespace Scripting {

    class Tile;
    class Wallpaper;

    class InfrastructureFactory {
      // Phase in new static convention here
      static const std::string GREY_SYSTEM_WALLPAPER;
      static constexpr const char* TILE_SYSTEM_PATH = "system/flooring/";
      static constexpr const char* TILE_ASSETS_PATH = "assets/flooring/";
      static constexpr const char* WALL_ASSETS_PATH = "assets/wallpaper/";
      static constexpr const char* TILE_SYSTEM_ROOT = "base.json";
      static constexpr const char* WALL_SYSTEM_ROOT = "base.json";
      Json::Value tileConstants;
      std::map< std::string, Threading::Lockable< Tile > > tileRegistry;
      std::map< std::string, std::shared_ptr< Wallpaper > > wallpaperRegistry;

      private:
        void registerFloorTile( const std::string& path );
        void registerWallpaper( const std::string& path );
        std::string getVariableOrValue( const std::string& key, const std::string& value );

      public:
        struct CannotLoadFileException : public std::exception { const char* what () const throw () { return "Could not load a required file!"; } };

        Threading::Lockable< Tile > getFloorTile( const std::string& key );
        std::shared_ptr< Wallpaper > getWallpaper( const std::string& key );

        void registerFloorTiles();
        void registerWallpapers();

    };

  }
}


#endif
