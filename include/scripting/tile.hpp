#ifndef FLOORTILE
#define FLOORTILE

#include <string>
#include <vector>
#include <jsoncpp/json/json.h>

namespace BlueBear {
  namespace Scripting {

    struct Tile {
      std::string soundPath;
      std::string imagePath;

      Tile( const std::string& soundPath, const std::string& imagePath ) : soundPath( soundPath ), imagePath( imagePath ) {} 
    };

  }
}


#endif
