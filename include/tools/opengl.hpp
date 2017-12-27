#ifndef OPENGL_TOOLS
#define OPENGL_TOOLS

#include "exceptions/genexc.hpp"
#include <GL/glew.h>
#include <string>
#include <exception>
#include <functional>
#include <mutex>

namespace BlueBear {
  namespace Tools {

    class OpenGL {
      static std::mutex mutex;

    public:
      EXCEPTION_TYPE( IncorrectShaderException, "Applied uniform to incorrect shader!" );

      static GLint getUniformLocation( const std::string& uniform );
      static GLint getUniformID( const std::string& uniform );
      static void lock( std::function< void() > predicate );
    };

  }
}


#endif
