#include "graphics/texture.hpp"
#include "tools/opengl.hpp"
#include "log.hpp"

namespace BlueBear {
  namespace Graphics {

    Texture::Texture( const sf::Image& texture ) {
      prepareTextureFromImage( texture );
    }

    Texture::Texture( const std::string& texFromFile, bool defer ) {
      deferred = std::make_unique< sf::Image >();
      if( !deferred->loadFromFile( texFromFile ) ) {
        throw ImageLoadFailureException();
      }

      if( !defer ) {
        sendDeferred();
      }
    }

    Texture::Texture( const glm::uvec2& dimensions, const GLvoid* data ) {
      prepareTextureFromData( dimensions, data );
    }

    Texture::~Texture() {
      if( !deferred ) {
        glDeleteTextures( 1, &id );
      }
    }

    void Texture::prepareTextureFromImage( const sf::Image& texture ) {
      glGenTextures( 1, &id );
      glBindTexture( GL_TEXTURE_2D, id );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
        auto size = texture.getSize();
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.getPixelsPtr() );
        glGenerateMipmap( GL_TEXTURE_2D );
      glBindTexture( GL_TEXTURE_2D, 0 );
    }

    void Texture::prepareTextureFromData( const glm::uvec2& size, const GLvoid* data ) {
      glGenTextures( 1, &id );
      glBindTexture( GL_TEXTURE_2D, id );
        glTexImage2D( GL_TEXTURE_2D, 0, GL_R32F, size.x, size.y, 0, GL_RED, GL_FLOAT, data );
      glBindTexture( GL_TEXTURE_2D, 0 );
    }

    void Texture::sendDeferred() {
      if( deferred ) {
        prepareTextureFromImage( *deferred );
        deferred = nullptr;
      }
    }
  }
}
