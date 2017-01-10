#include "tools/opengl.hpp"

namespace BlueBear {
  namespace Tools {

    GLint OpenGL::getUniformLocation( const std::string& uniform ) {
      GLint currentShader = 0;
      glGetIntegerv( GL_CURRENT_PROGRAM, &currentShader );

      GLint uniformLocation = glGetUniformLocation( currentShader, uniform.c_str() );
      if( uniformLocation == -1 ) {
        throw IncorrectShaderException();
      }

      return uniformLocation;
    }

  }
}
