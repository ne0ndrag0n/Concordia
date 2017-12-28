#include "tools/opengl.hpp"
#include "graphics/shader.hpp"

namespace BlueBear {
  namespace Tools {

    GLint OpenGL::getUniformID( const std::string& uniform ) {
      return glGetUniformLocation( Graphics::Shader::CURRENT_PROGRAM, uniform.c_str() );
    }

    GLint OpenGL::getUniformLocation( const std::string& uniform ) {
      GLint uniformLocation = OpenGL::getUniformID( uniform );
      if( uniformLocation == -1 ) {
        throw IncorrectShaderException();
      }

      return uniformLocation;
    }

  }
}
