#ifndef INFRASTRUCTUREFACTORY
#define INFRASTRUCTUREFACTORY

#include <string>
#include <exception>
#include <memory>
#include <map>

namespace BlueBear {
  namespace Scripting {

    class Tile;
    class Wallpaper;

    class InfrastructureFactory {
      static constexpr const char* TILE_SYSTEM_PATH = "system/flooring/";
      static constexpr const char* TILE_ASSETS_PATH = "assets/flooring/";
      static constexpr const char* TILE_SYSTEM_ROOT = "base.json";
      std::map< std::string, std::shared_ptr< Tile > > tileRegistry;

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
