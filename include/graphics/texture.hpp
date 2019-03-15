#ifndef GFXTEXTURE
#define GFXTEXTURE

#include "exceptions/genexc.hpp"
#include <GL/glew.h>
#include <assimp/types.h>
#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>
#include <string>
#include <memory>

namespace BlueBear {
  namespace Graphics {

    class Texture {
        std::unique_ptr< sf::Image > deferred;

        Texture( const Texture& );
        Texture& operator=( const Texture& );

        void prepareTextureFromImage( const sf::Image& texture );
        void prepareTextureFromData( const glm::uvec2& size, const GLvoid* data );

      public:
        EXCEPTION_TYPE( ImageLoadFailureException, "Image could not be loaded!" );
        GLuint id;

        Texture( const sf::Image& texture );
        Texture( const glm::uvec2& dimensions, const GLvoid* data );
        Texture( const std::string& texFromFile, bool defer = false );
        ~Texture();

        void sendDeferred();
    };
  }
}
#endif
