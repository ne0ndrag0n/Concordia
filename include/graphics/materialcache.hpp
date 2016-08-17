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
        std::shared_ptr< Material > get( Scripting::Tile& tile );
        void prune();
    };
  }
}


#endif
