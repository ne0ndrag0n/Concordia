#ifndef OPENGL_TOOLS
#define OPENGL_TOOLS

#include "exceptions/genexc.hpp"
#include <GL/glew.h>
#include <string>
#include <exception>

namespace BlueBear {
  namespace Tools {

    class OpenGL {
    public:
      EXCEPTION_TYPE( IncorrectShaderException, "Applied uniform to incorrect shader!" );

      static GLint getUniformLocation( const std::string& uniform );
      static GLint getUniformID( const std::string& uniform );
    };

  }
}


#endif
