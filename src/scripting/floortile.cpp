#include "scripting/floortile.hpp"
#include <jsoncpp/json/json.h>
#include <string>
#include <fstream>
#include <exception>

namespace BlueBear {
  namespace Scripting {

    bool FloorTile::baseLoaded = false;
    Json::Value FloorTile::baseClasses;

    FloorTile::FloorTile( const std::string& jsonPath ) {
      // Make sure base JSON is loaded so we can properly extend
      loadBase();

      std::ifstream tileJson;
      tileJson.exceptions( std::ios::failbit | std::ios::badbit );
      tileJson.open( jsonPath );

      Json::Value floorDefinitions;
      Json::Reader reader;

      if( !reader.parse( tileJson, floorDefinitions ) ) {
        throw FloorTile::CannotLoadFileException();
      }

      Json::Value usableDefinition;

      // Determine if extension is required
      if( floorDefinitions.isMember( "extend" ) && floorDefinitions[ "extend" ].isString() ) {
        // Grab a copy of a base class from FloorTile::baseClasses, and extend floorDefinition over it
        std::string extensionKey = floorDefinitions.asString();
        // It better exist
        if( FloorTile::baseClasses.isMember( extensionKey ) && FloorTile::baseClasses.isObject() ) {
          usableDefinition = FloorTile::baseClasses[ extensionKey ];
          // absolutely disgusting
          for( Json::Value::iterator jsonIterator = floorDefinitions.begin(); jsonIterator != floorDefinitions.end(); ++jsonIterator ) {
            Json::Value key = jsonIterator.key();
            Json::Value value = *jsonIterator;

            usableDefinition[ key.asString() ] = value;
          }
        } else {
          throw FloorTile::JsonClassNotFoundException();
        }
      } else {
        // No extension is required
        usableDefinition = floorDefinitions;
      }

      // Retrieve all properties out of usableDefinition
    }

    void FloorTile::loadBase() {
      if( baseLoaded == false ) {
        std::ifstream base;
        base.exceptions( std::ios::failbit | std::ios::badbit );
        base.open( FloorTile::BASE_JSON_PATH );

        Json::Reader reader;
        if( !reader.parse( base, FloorTile::baseClasses ) ) {
          throw FloorTile::CannotLoadFileException();
        }

        baseLoaded = true;
      }
    }

  }
}
