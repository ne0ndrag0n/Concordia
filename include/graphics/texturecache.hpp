#ifndef TEXTURECACHE
#define TEXTURECACHE

#include <string>
#include <map>
#include <memory>

namespace BlueBear {
  namespace Graphics {
    class Texture;

    class TextureCache {
      std::map< std::string, std::shared_ptr< Texture > > textureCache;

      public:
        std::shared_ptr< Texture > get( const std::string& path );
        void prune();
    };

  }
}

#endif
