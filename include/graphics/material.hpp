#ifndef GFXMATERIAL
#define GFXMATERIAL

#include <vector>
#include <memory>
#include <GL/glew.h>

namespace BlueBear {
  namespace Graphics {
    class Texture;

    using TextureList = std::vector< std::shared_ptr< Texture > >;

    class Material {

      public:
        TextureList diffuseTextures;
        TextureList specularTextures;

        Material();
        Material( const std::shared_ptr< Texture >& diffuseTexture );
        Material( TextureList diffuseTextures );
        Material( TextureList diffuseTextures, TextureList specularTextures );

        void sendToShader();
    };
  }
}
#endif
