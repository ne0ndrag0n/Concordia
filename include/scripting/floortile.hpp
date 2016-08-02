#ifndef FLOORTILE
#define FLOORTILE

#include <string>
#include <vector>
#include <jsoncpp/json/json.h>

namespace BlueBear {
  namespace Scripting {

    struct FloorTile {
      // This will change as we lay out sf::Sound
      std::string soundPath;
      unsigned char saturation;
      std::vector< std::string > sequence;
    };

  }
}


#endif
