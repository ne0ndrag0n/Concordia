#ifndef OPENGL_TOOLS
#define OPENGL_TOOLS

#include <GL/glew.h>
#include <string>
#include <exception>

namespace BlueBear {
  namespace Tools {

    class OpenGL {
    public:
      static GLint getUniformLocation( const std::string& uniform );

      struct IncorrectShaderException : public std::exception {

        const char* what() const throw() {
          return "Applied uniform to incorrect shader!";
        }

      };

    };

  }
}


#endif
