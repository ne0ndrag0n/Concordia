#include "graphics/texture.hpp"
#include "log.hpp"
#include <assimp/types.h>
#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <sstream>

namespace BlueBear {
  namespace Graphics {

    Texture::Texture( GLuint id, aiString path ) :
      id( id ), path( path ) {}

    Texture::Texture( sf::Image& texture ) {
      prepareTextureFromImage( texture );
    }

    Texture::Texture( std::string texFromFile ) {
      sf::Image texture;
      if( !texture.loadFromFile( texFromFile ) ) {
        std::stringstream stream( "Couldn't load texture " );
        stream << texFromFile;
        Log::getInstance().error( "Texture::Texture", stream.str() );
        return;
      }

      prepareTextureFromImage( texture );
    }

    Texture::~Texture() {
      glDeleteTextures( 1, &id );
    }

    void Texture::prepareTextureFromImage( sf::Image& texture ) {
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
  }
}
