#include "scripting/infrastructurefactory.hpp"
#include "scripting/tile.hpp"
#include "log.hpp"
#include "utility.hpp"
#include <jsoncpp/json/json.h>
#include <string>
#include <fstream>
#include <memory>
#include <exception>
#include <vector>

namespace BlueBear {
  namespace Scripting {

    /**
     * Given the key and value, check against tileConstants for a value. If the value is present within, return THAT value,
     * otherwise return value.
     */
    std::string InfrastructureFactory::getVariableOrValue( const std::string& key, const std::string& value ) {
      if( tileConstants.isMember( key ) ) {
        Json::Value constants = tileConstants[ key ];
        // If the value is in here, return it
        if( constants.isMember( value ) ) {
          return constants[ value ].asString();
        }
      }

      return value;
    }

    void InfrastructureFactory::registerFloorTile( const std::string& fullPath ) {
      std::ifstream definitionFile;
      definitionFile.exceptions( std::ios::failbit | std::ios::badbit );
      definitionFile.open( fullPath );

      Json::Reader reader;
      Json::Value definitionJSON;
      if( reader.parse( definitionFile, definitionJSON ) ) {
        for( Json::Value::iterator jsonIterator = definitionJSON.begin(); jsonIterator != definitionJSON.end(); ++jsonIterator ) {
          std::string key = jsonIterator.key().asString();
          Json::Value tileDefinition = *jsonIterator;

          if( tileDefinition.isMember( "sound" ) && tileDefinition.isMember( "image" ) ) {
            tileRegistry[ key ] = std::make_shared< Tile >(
              getVariableOrValue( "sound", tileDefinition[ "sound" ].asString() ),
              getVariableOrValue( "image", tileDefinition[ "image" ].asString() )
            );
          } else {
            Log::getInstance().error( "InfrastructureFactory::registerFloorTile", "Unable to create floor tile at path " + fullPath + ": missing fields." );
          }
        }
      } else {
        Log::getInstance().error( "InfrastructureFactory::registerFloorTile", "Unable to parse JSON file " + fullPath );
      }
    }

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
      std::vector< std::string > directories = Utility::getSubdirectoryList( TILE_ASSETS_PATH );
      for( std::string& directory : directories ) {
        registerFloorTile( std::string( TILE_ASSETS_PATH ) + directory + "/base.json" );
      }
    }

  }
}
