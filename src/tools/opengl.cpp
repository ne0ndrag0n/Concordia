#include "tools/opengl.hpp"
#include "graphics/shader.hpp"

namespace BlueBear {
  namespace Tools {

    GLint OpenGL::getUniformLocation( const std::string& uniform ) {
      GLint uniformLocation = glGetUniformLocation( Graphics::Shader::CURRENT_PROGRAM, uniform.c_str() );
      if( uniformLocation == -1 ) {
        throw IncorrectShaderException();
      }

      return uniformLocation;
    }

  }
}
