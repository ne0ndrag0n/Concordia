#ifndef INFRASTRUCTUREFACTORY
#define INFRASTRUCTUREFACTORY

#include <string>
#include <jsoncpp/json/json.h>
#include <exception>

namespace BlueBear {
  namespace Scripting {

    class Tile;
    class Wallpaper;

    class InfrastructureFactory {
      static constexpr const char* TILE_SYSTEM_PATH = "system/flooring/";
      static constexpr const char* TILE_ASSETS_PATH = "assets/flooring/";
      static constexpr const char* TILE_SYSTEM_ROOT = "base.json";
      Json::Value tileConstants;

      public:
        struct CannotLoadFileException : public std::exception { const char* what () const throw () { return "Could not load a required file!"; } };

        Tile getFloorTile( const std::string& key );
        Wallpaper getWallpaper( const std::string& key );

        void registerFloorTiles();
        void registerWallpaper();

    };

  }
}


#endif
