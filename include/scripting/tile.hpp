#ifndef FLOORTILE
#define FLOORTILE

#include <string>
#include <vector>
#include <json/json.h>

namespace BlueBear {
  namespace Scripting {

    struct Tile {
      std::string id;
      std::string soundPath;
      std::string imagePath;
      double tilePrice;

      Tile( const std::string& id, const std::string& soundPath, const std::string& imagePath, double tilePrice ) : id( id ), soundPath( soundPath ), imagePath( imagePath ), tilePrice( tilePrice ) {}
    };

  }
}


#endif
