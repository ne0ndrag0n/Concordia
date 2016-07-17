#include "graphics/atlasbuilder.hpp"
#include "graphics/texture.hpp"
#include <fstream>
#include <string>
#include <jsoncpp/json/json.h>
#include <SFML/Graphics.hpp>
#include <exception>
#include <utility>
#include <memory>

namespace BlueBear {
  namespace Graphics {

    void AtlasBuilder::setAtlasMapping( const std::string& key, const std::string& path ) {
      AtlasMapping& mapping = mappings.at( key );

      mapping.imagePath = path;
    }

    void AtlasBuilder::configure( const std::string& jsonPath ) {
      mappings.clear();

      std::ifstream schemaFile;
      // std::ifstream::failure
      schemaFile.exceptions( std::ios::failbit | std::ios::badbit );
      schemaFile.open( jsonPath );

      Json::Value schema;
      Json::Reader reader;

      if( !reader.parse( schemaFile, schema ) ) {
        throw AtlasBuilder::CannotLoadFileException();
      }

      // Create a base image on which to overlay existing images
      Json::Value baseProps = schema[ "base" ];
      Json::Value components = schema[ "mappings" ];

      // Dispose of any old image
      base = sf::Image();
      if( baseProps[ "image" ].isString() ) {
        if( !base.loadFromFile( baseProps[ "image" ].asString() ) ) {
          throw AtlasBuilder::CannotLoadFileException();
        }
      } else {
        base.create( baseProps[ "width" ].asInt(), baseProps[ "height" ].asInt() );
      }

      // Load all components into mappings
      for( Json::Value::iterator jsonIterator = components.begin(); jsonIterator != components.end(); ++jsonIterator ) {
        std::string key = jsonIterator.key().asString();
        Json::Value value = *jsonIterator;

        mappings[ key ] = AtlasMapping{
          ( unsigned int ) value[ "x" ].asInt(),
          ( unsigned int ) value[ "y" ].asInt(),
          ( unsigned int ) value[ "width" ].asInt(),
          ( unsigned int ) value[ "height" ].asInt(),
          ""
        };
      }
    }

    std::shared_ptr< Texture > AtlasBuilder::getTextureAtlas() {
      sf::Image atlasBase = base;

      // Apply each overlay
      for( auto& pair : mappings ) {
        sf::Image overlay;
        AtlasMapping mapping = pair.second;

        if( !overlay.loadFromFile( mapping.imagePath ) ) {
          throw AtlasBuilder::CannotLoadFileException();
        }

        atlasBase.copy( overlay, mapping.x, mapping.y );
      }

      // Overlay this sf::Image into an OpenGL texture
      return std::make_shared< Texture >( atlasBase );
    }
  }
}
