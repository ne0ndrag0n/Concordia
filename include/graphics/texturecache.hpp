#ifndef TEXTURECACHE
#define TEXTURECACHE

#include <string>
#include <map>
#include <memory>
#include "graphics/atlasbuilder.hpp"
#include "graphics/imagebuilder/imagesource.hpp"

namespace BlueBear {
  namespace Graphics {
    class Texture;

    class TextureCache {
      using SharedPointerTextureCache = std::map< std::string, std::shared_ptr< Texture > >;
      using AtlasSettings = std::map< std::string, std::unique_ptr< ImageSource > >;

      struct AtlasBuilderEntry {
        AtlasBuilder builder;
        SharedPointerTextureCache generatedTextures;
      };

      SharedPointerTextureCache textureCache;
      std::map< std::string, AtlasBuilderEntry > atlasTextureCache;

      std::shared_ptr< Texture > generateForAtlasBuilderEntry( AtlasBuilderEntry& entry, AtlasSettings& mappings );

      public:
        std::shared_ptr< Texture > get( const std::string& path );
        // Order matters here in the map if you want to get the performance benefit!
        // The map will be transformed to a single string used as the key for the textureCache map
        std::shared_ptr< Texture > getUsingAtlas( const std::string& atlasBasePath, AtlasSettings& mappings );
        void prune();

      private:
        std::string getKey( AtlasSettings& mappings );
    };

  }
}

#endif
