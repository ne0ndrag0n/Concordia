#include "tools/opengl.hpp"
#include "graphics/shader.hpp"
#include "log.hpp"
#include <glm/gtc/type_ptr.hpp>

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
      }
    }

    void OpenGL::setUniform( const std::string& id, const int value ) {
      GLint uniformLocation = OpenGL::getUniformID( id );
      if( uniformLocation != -1 ) {
        glUniform1i( uniformLocation, value );
      }
    }

    void OpenGL::setUniform( const std::string& id, const unsigned int value ) {
      GLint uniformLocation = OpenGL::getUniformID( id );
      if( uniformLocation != -1 ) {
        glUniform1ui( uniformLocation, value );
      }
    }

    void OpenGL::setUniform( const std::string& id, const float value ) {
      GLint uniformLocation = OpenGL::getUniformID( id );
      if( uniformLocation != -1 ) {
        glUniform1f( uniformLocation, value );
      }
    }

    void OpenGL::setUniform( const std::string& id, const glm::mat4& value ) {
      GLint uniformLocation = OpenGL::getUniformID( id );
      if( uniformLocation != -1 ) {
        glUniformMatrix4fv( uniformLocation, 1, GL_FALSE, glm::value_ptr( value ) );
      }
    }

    void OpenGL::setUniform( const std::string& id, unsigned int size, const GLfloat* value ) {
      GLint uniformLocation = OpenGL::getUniformID( id );
      if( uniformLocation != -1 ) {
        glUniformMatrix4fv( uniformLocation, size, GL_FALSE, value );
      }
    }

  }
}
