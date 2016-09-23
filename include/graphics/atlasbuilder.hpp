#ifndef ATLASBUILDER
#define ATLASBUILDER

#include "graphics/imagebuilder/imagebuilder.hpp"
#include <istream>
#include <string>
#include <SFML/Graphics.hpp>
#include <exception>
#include <utility>
#include <memory>

namespace BlueBear {
  namespace Graphics {

    class Texture;

    /**
     * Builds a texture atlas from a given JSON schema and a base image.
     */
    class AtlasBuilder {

      private:
        struct AtlasMapping {
          unsigned int x;
          unsigned int y;
          unsigned int width;
          unsigned int height;
          std::unique_ptr< ImageBuilder > imageBuilder;
        };

        sf::Image base;
        std::map< std::string, AtlasMapping > mappings;

      public:
        struct CannotLoadFileException : public std::exception { const char* what () const throw () { return "Could not load a required file!"; } };

        AtlasBuilder() = default;

        void setAtlasMapping( const std::string& key, std::unique_ptr< ImageBuilder > builder );
        void configure( const std::string& jsonPath );

        std::shared_ptr< Texture > getTextureAtlas();
    };

  }
}

#endif
