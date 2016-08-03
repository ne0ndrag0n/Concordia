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
    };

  }
}


#endif
