#ifndef INFRASTRUCTUREFACTORY
#define INFRASTRUCTUREFACTORY

#include <string>
#include <jsoncpp/json/json.h>
#include <exception>

namespace BlueBear {
  namespace Scripting {

    class FloorTile;
    class Wallpaper;

    class InfrastructureFactory {
      static constexpr const char* FLOORTILES_REGISTRY_PATH = "assets/flooring/";
      static constexpr const char* FLOORTILES_ROOT_FILE = "classes.json";
      Json::Value rootFloorClasses;

      public:
        struct CannotLoadFileException : public std::exception { const char* what () const throw () { return "Could not load a required file!"; } };

        FloorTile getFloorTile( const std::string& key );
        Wallpaper getWallpaper( const std::string& key );

        void registerFloorTiles();
        void registerWallpaper();

    };

  }
}


#endif
