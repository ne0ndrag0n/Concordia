#ifndef OPENGL_TOOLS
#define OPENGL_TOOLS

#include "exceptions/genexc.hpp"
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <string>
#include <exception>
#include <array>
#include <optional>
#include <vector>

namespace BlueBear {
  namespace Tools {

    class OpenGL {
      static std::array< bool, 16 > textureUnits;

    public:
      EXCEPTION_TYPE( IncorrectShaderException, "Applied uniform to incorrect shader!" );

      static std::optional< unsigned int > getTextureUnit();
      static void returnTextureUnits( const std::vector< unsigned int >& items );

      static GLint getUniformLocation( const std::string& uniform );
      static GLint getUniformID( const std::string& uniform );

      static void setUniform( const std::string& id, const glm::vec3& value );
      static void setUniform( const std::string& id, const int value );
      static void setUniform( const std::string& id, const unsigned int value );
      static void setUniform( const std::string& id, const float value );
      static void setUniform( const std::string& id, const glm::mat4& value );
      static void setUniform( const std::string& id, unsigned int size, const GLfloat* value );
    };

  }
}


#endif
