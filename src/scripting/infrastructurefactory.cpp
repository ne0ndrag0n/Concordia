#include "scripting/infrastructurefactory.hpp"
#include "scripting/tile.hpp"
#include "scripting/wallpaper.hpp"
#include "log.hpp"
#include "tools/utility.hpp"
#include "threading/lockable.hpp"
#include <algorithm>
#include <jsoncpp/json/json.h>
#include <string>
#include <fstream>
#include <memory>
#include <exception>
#include <vector>

namespace BlueBear {
  namespace Scripting {

    const std::string InfrastructureFactory::GREY_SYSTEM_WALLPAPER = "system/models/wall/greywallpaper.png";

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
            tileRegistry[ key ] = Threading::Lockable< Tile >::create(
              key,
              path + "/" + getVariableOrValue( "sound", tileDefinition[ "sound" ].asString() ),
              path + "/" + getVariableOrValue( "image", tileDefinition[ "image" ].asString() ),
              tileDefinition[ "price" ].asDouble()
            );

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
      std::vector< std::string > directories = Tools::Utility::getSubdirectoryList( TILE_ASSETS_PATH );
      std::for_each( directories.begin(), directories.end(), [ & ]( std::string& directory ) {
        registerFloorTile( std::string( TILE_ASSETS_PATH ) + directory );
      } );
    }

    /**
     * Throw an exception if the entry does not exist, crash the game gracefully so there's no undefined behavior.
     */
    Threading::Lockable< Tile > InfrastructureFactory::getFloorTile( const std::string& key ) {
      return tileRegistry.at( key );
    }

    Threading::Lockable< Wallpaper > InfrastructureFactory::getWallpaper( const std::string& key ) {
      return wallpaperRegistry.at( key );
    }

    /**
     * Register all the wallpaper. This is markedly simpler since there's no need for a base wallpaper class.
     * The only important thing this function needs to make sure of, is that there's always a "grey" wallpaper class.
     * Wallpaper types in assets/wallpaper that define themselves with id "grey" will be silently ignored, and Log
     * will throw a warning that this id is reserved for the unpainted wallpanel class.
     */
    void InfrastructureFactory::registerWallpapers() {
      // First and foremost - register _grey, the standard, hardcoded grey wallaper.
      // No more constexpr - GCC bug breaks them randomly
      wallpaperRegistry[ "_grey" ] = Threading::Lockable< Wallpaper >::create( "_grey", GREY_SYSTEM_WALLPAPER, 0.0 );

      std::vector< std::string > directories = Tools::Utility::getSubdirectoryList( WALL_ASSETS_PATH );
      std::for_each( directories.begin(), directories.end(), [ & ]( std::string& directory ) {
        registerWallpaper( std::string( WALL_ASSETS_PATH ) + directory );
      } );
    }

    void InfrastructureFactory::registerWallpaper( const std::string& path ) {
      std::string fullPath = path + "/" + WALL_SYSTEM_ROOT;
      std::ifstream definitionFile;
      definitionFile.exceptions( std::ios::failbit | std::ios::badbit );
      definitionFile.open( fullPath );

      Json::Reader reader;
      Json::Value definitionJSON;
      if( reader.parse( definitionFile, definitionJSON ) ) {
        for( Json::Value::iterator jsonIterator = definitionJSON.begin(); jsonIterator != definitionJSON.end(); ++jsonIterator ) {
          std::string key = jsonIterator.key().asString();

          if( key != "_grey" ) {
            Json::Value wallpaperDefinition = *jsonIterator;

            if( wallpaperDefinition.isMember( "image" ) ) {
              wallpaperRegistry[ key ] = Threading::Lockable< Wallpaper >::create(
                key,
                path + "/" + wallpaperDefinition[ "image" ].asString(),
                wallpaperDefinition[ "price" ].asDouble()
              );

              Log::getInstance().debug( "InfrastructureFactory::registerWallpaper" , "Registered " + key + " at " + fullPath );
            } else {
              Log::getInstance().error( "InfrastructureFactory::registerWallpaper", "Unable to create wallpaper at path " + fullPath + ": missing fields." );
            }
          } else {
            Log::getInstance().warn( "InfrastructureFactory::registerWallpaper", "Cannot register wallpaper: \"_grey\" is a reserved id." );
          }
        }
      } else {
        Log::getInstance().error( "InfrastructureFactory::wallpaper", "Unable to parse JSON file " + fullPath );
      }
    }
  }
}
