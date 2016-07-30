#ifndef FLOORTILE
#define FLOORTILE

#include <string>
#include <vector>

namespace BlueBear {
  namespace Scripting {

    class FloorTile {
      public:
        // This will change as we lay out sf::Sound
        std::string soundPath;
        unsigned char saturation;
        unsigned char currentSaturation;
        unsigned char currentSequence;
        std::vector< std::string > sequenceImagePaths;

        FloorTile::FloorTile( const std::string& jsonPath );
    };

  }
}


#endif
