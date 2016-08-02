#include "scripting/infrastructurefactory.hpp"
#include "scripting/floortile.hpp"
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
      std::ifstream classesFile;
      classesFile.exceptions( std::ios::failbit | std::ios::badbit );
      classesFile.open( std::string( FLOORTILES_REGISTRY_PATH ) + FLOORTILES_ROOT_FILE );

      Json::Reader reader;
      if( !reader.parse( classesFile, rootFloorClasses ) ) {
        throw InfrastructureFactory::CannotLoadFileException();
      }

      // Traverse directories in FLOORTILES_REGISTRY_PATH, loading definition.json within
    }

  }
}
