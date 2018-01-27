#ifndef OPENGL_TOOLS
#define OPENGL_TOOLS

#include "exceptions/genexc.hpp"
#include <glm/glm.hpp>
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

      static void setUniform( const std::string& id, const glm::vec3& value );
      static void setUniform( const std::string& id, const int value );
      static void setUniform( const std::string& id, const float value );
    };

  }
}


#endif
