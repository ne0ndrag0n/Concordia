#include "scripting/infrastructurefactory.hpp"
#include "scripting/tile.hpp"
#include "log.hpp"
#include "utility.hpp"
#include "threading/async.hpp"
#include <async++.h>
#include <jsoncpp/json/json.h>
#include <string>
#include <fstream>
#include <memory>
#include <exception>
#include <vector>
#include <mutex>

namespace BlueBear {
  namespace Scripting {

    /**
     * Given the key and value, check against tileConstants for a value. If the value is present within, return THAT value,
     * otherwise return value.
     *
     * JSONCPP MAY NOT BE THREAD SAFE
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

    void InfrastructureFactory::registerFloorTile( const std::string& path ) {
      std::string fullPath = path + "/" + TILE_SYSTEM_ROOT;
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
            {
              std::unique_lock< std::mutex > tileRegistryLock( tileRegistryMutex );
              tileRegistry[ key ] = std::make_shared< Tile >(
                key,
                path + "/" + getVariableOrValue( "sound", tileDefinition[ "sound" ].asString() ),
                path + "/" + getVariableOrValue( "image", tileDefinition[ "image" ].asString() ),
                tileDefinition[ "price" ].asDouble()
              );
            }

            Log::getInstance().debug( "InfrastructureFactory::registerFloorTile" , "Registered " + key + " at " + fullPath );
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
      async::parallel_for( Threading::AsyncManager::getInstance().getScheduler(), directories, [ & ]( std::string& directory ) {
        registerFloorTile( std::string( TILE_ASSETS_PATH ) + directory );
      } );
    }

    /**
     * Throw an exception if the entry does not exist, crash the game gracefully so there's no undefined behavior.
     */
    std::shared_ptr< Tile > InfrastructureFactory::getFloorTile( const std::string& key ) {
      return tileRegistry.at( key );
    }

  }
}
