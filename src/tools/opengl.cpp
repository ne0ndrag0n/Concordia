#include "tools/opengl.hpp"
#include "graphics/shader.hpp"
#include "log.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace BlueBear {
  namespace Tools {

    std::array< bool, 16 > OpenGL::textureUnits{ false };
    std::unordered_map< GLint, std::unordered_map< std::string, GLint > >  OpenGL::cachedUniformIds;

    std::optional< unsigned int > OpenGL::getTextureUnit() {
      for( unsigned int i = 0; i != 16; i++ ) {
        if( textureUnits[ i ] == false ) {
          textureUnits[ i ] = true;
          return i;
        }
      }

      return {};
    }

    void OpenGL::returnTextureUnits( const std::vector< unsigned int >& items ) {
      for( unsigned int index : items ) {
        textureUnits[ index ] = false;
      }
    }

    GLint OpenGL::getUniformID( const std::string& uniform ) {
      auto& bucket = cachedUniformIds[ Graphics::Shader::CURRENT_PROGRAM ];
      auto it = bucket.find( uniform );
      if( it != bucket.end() ) {
        // Seen before
        return it->second;
      }

      return bucket[ uniform ] = glGetUniformLocation( Graphics::Shader::CURRENT_PROGRAM, uniform.c_str() );
    }

    GLint OpenGL::getUniformLocation( const std::string& uniform ) {
      GLint uniformLocation = OpenGL::getUniformID( uniform );
      if( uniformLocation == -1 ) {
        Log::getInstance().error( "OpenGL::getUniformLocation", uniform + " doesn't exist on the current shader!" );
        throw IncorrectShaderException();
      }

      return uniformLocation;
    }

  }
}
