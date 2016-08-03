#include "scripting/infrastructurefactory.hpp"
#include "scripting/tile.hpp"
#include <jsoncpp/json/json.h>
#include <string>
#include <fstream>
#include <exception>

namespace BlueBear {
  namespace Scripting {

    /**
     * Load floor tile definitions into a large JSON map, using root classes to build them if needed
     *
     * @throws CannotLoadFileException if a required file could not be loaded.
     */
    void InfrastructureFactory::registerFloorTiles() {
      // Load the root floor classes
      std::ifstream tileConstantsFile;
      tileConstantsFile.exceptions( std::ios::failbit | std::ios::badbit );
      tileConstantsFile.open( std::string( TILE_SYSTEM_PATH ) + TILE_SYSTEM_ROOT );

      Json::Reader reader;
      if( !reader.parse( tileConstantsFile, tileConstants ) ) {
        throw InfrastructureFactory::CannotLoadFileException();
      }

      // Now that all constants are loaded, start traversing the directory and get the user-defined packages

    }

  }
}
