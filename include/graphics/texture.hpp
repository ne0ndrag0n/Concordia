#ifndef GFXTEXTURE
#define GFXTEXTURE

#include <GL/glew.h>
#include <assimp/types.h>
#include <string>
#include <SFML/Graphics.hpp>

namespace BlueBear {
  namespace Graphics {

    class Texture {
      private:
        Texture( const Texture& );
        Texture& operator=( const Texture& );

        void prepareTextureFromImage( sf::Image& texture );

      public:
        Texture( GLuint id, aiString path );
        Texture( sf::Image& texture );
        Texture( std::string texFromFile );
        ~Texture();
        GLuint id = -1;
        aiString path;
    };
  }
}
#endif
