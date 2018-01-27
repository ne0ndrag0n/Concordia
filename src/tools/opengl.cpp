#include "tools/opengl.hpp"
#include "graphics/shader.hpp"
#include "log.hpp"

namespace BlueBear {
  namespace Tools {

    GLint OpenGL::getUniformID( const std::string& uniform ) {
      return glGetUniformLocation( Graphics::Shader::CURRENT_PROGRAM, uniform.c_str() );
    }

    GLint OpenGL::getUniformLocation( const std::string& uniform ) {
      GLint uniformLocation = OpenGL::getUniformID( uniform );
      if( uniformLocation == -1 ) {
        Log::getInstance().error( "OpenGL::getUniformLocation", uniform + " doesn't exist on the current shader!" );
        throw IncorrectShaderException();
      }

      return uniformLocation;
    }

    void OpenGL::setUniform( const std::string& id, const glm::vec3& value ) {
      GLint uniformLocation = OpenGL::getUniformID( id );
      if( uniformLocation != -1 ) {
        glUniform3f( uniformLocation, value[ 0 ], value[ 1 ], value[ 2 ] );
      } else {
        Log::getInstance().warn( "OpenGL::setUniform", id + " undefined or unused in current shader; not setting uniform." );
      }
    }

    void OpenGL::setUniform( const std::string& id, const int value ) {
      GLint uniformLocation = OpenGL::getUniformID( id );
      if( uniformLocation != -1 ) {
        glUniform1i( uniformLocation, value );
      } else {
        Log::getInstance().warn( "OpenGL::setUniform", id + " undefined or unused in current shader; not setting uniform." );
      }
    }

    void OpenGL::setUniform( const std::string& id, const float value ) {
      GLint uniformLocation = OpenGL::getUniformID( id );
      if( uniformLocation != -1 ) {
        glUniform1f( uniformLocation, value );
      } else {
        Log::getInstance().warn( "OpenGL::setUniform", id + " undefined or unused in current shader; not setting uniform." );
      }
    }

  }
}
