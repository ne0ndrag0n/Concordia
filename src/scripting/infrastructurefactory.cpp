#include "scripting/infrastructurefactory.hpp"
#include "scripting/tile.hpp"
#include "log.hpp"
#include "utility.hpp"
#include <jsoncpp/json/json.h>
#include <string>
#include <fstream>
#include <exception>
#include <vector>

namespace BlueBear {
  namespace Scripting {

    /**
     * Load floor tile definitions into a large JSON map, using root classes to build them if needed
     *
     * @throws CannotLoadFileException if a required file could not be loaded.
     */
    void InfrastructureFactory::registerFloorTiles() {
      // Load the root floor classes
      Json::Value tileConstants;
      std::ifstream tileConstantsFile;
      tileConstantsFile.exceptions( std::ios::failbit | std::ios::badbit );
      tileConstantsFile.open( std::string( TILE_SYSTEM_PATH ) + TILE_SYSTEM_ROOT );

      Json::Reader reader;
      if( !reader.parse( tileConstantsFile, tileConstants ) ) {
        throw InfrastructureFactory::CannotLoadFileException();
      }

      // Now that all constants are loaded, start traversing the directory and get the user-defined packages
      std::vector< std::string > directories = Utility::getSubdirectoryList( TILE_ASSETS_PATH );
      for( std::string& directory : directories ) {
        Log::getInstance().debug( "InfrastructureFactory", directory );
      }
    }

  }
}
