#ifndef TEXTURECACHE
#define TEXTURECACHE

#include <string>
#include <map>
#include <memory>
#include "graphics/atlasbuilder.hpp"

namespace BlueBear {
  namespace Graphics {
    class Texture;

    class TextureCache {
      using SharedPointerTextureCache = std::map< std::string, std::shared_ptr< Texture > >;
      using AtlasBuilderMappings = std::map< std::string, std::string >;

      struct AtlasBuilderEntry {
        AtlasBuilder builder;
        SharedPointerTextureCache generatedTextures;
      };

      SharedPointerTextureCache textureCache;
      std::map< std::string, AtlasBuilderEntry > atlasTextureCache;

      std::shared_ptr< Texture > generateForAtlasBuilderEntry( AtlasBuilderEntry& entry, const AtlasBuilderMappings& mappings );

      public:
        std::shared_ptr< Texture > get( const std::string& path );
        // Order matters here in the map if you want to get the performance benefit!
        // The map will be transformed to a single string used as the key for the textureCache map
        std::shared_ptr< Texture > getUsingAtlas( const std::string& atlasBasePath, const AtlasBuilderMappings& mappings );
        void prune();
    };

  }
}

#endif
