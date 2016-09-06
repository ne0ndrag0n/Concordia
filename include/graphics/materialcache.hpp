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
        struct AtlasPackage {
          std::string atlasJSONPath;
          std::map< std::string, std::string > mappings;
        };
        std::shared_ptr< Material > get( Scripting::Tile& tile );
        std::shared_ptr< Material > getAtlas( AtlasPackage& package );
        void prune();
    };
  }
}


#endif
