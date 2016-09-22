#ifndef MATERIALCACHE
#define MATERIALCACHE

#include "graphics/texturecache.hpp"
#include <string>
#include <map>
#include <memory>

namespace BlueBear {
  namespace Scripting {
    class Tile;
  }

  namespace Graphics {
    class Material;

    class MaterialCache {
      TextureCache textureCache;
      std::map< std::string, std::shared_ptr< Material > > materialCache;

      public:
        using AtlasSettings = std::map< std::string, std::string >;

        std::shared_ptr< Material > get( Scripting::Tile& tile );
        std::shared_ptr< Material > getAtlas( const std::string& atlasJSONPath, const AtlasSettings& mappings );

        void prune();

      private:
        std::string getKey( const AtlasSettings& mappings );
    };
  }
}


#endif
