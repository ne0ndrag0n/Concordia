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
#include <unordered_map>

namespace BlueBear {
  namespace Tools {

    class OpenGL {
      static std::array< bool, 16 > textureUnits;
      static std::unordered_map< GLint, std::unordered_map< std::string, GLint > > cachedUniformIds;

    public:
      EXCEPTION_TYPE( IncorrectShaderException, "Applied uniform to incorrect shader!" );

      static std::optional< unsigned int > getTextureUnit();
      static void returnTextureUnits( const std::vector< unsigned int >& items );

      static GLint getUniformLocation( const std::string& uniform );
      static GLint getUniformID( const std::string& uniform );
    };

  }
}


#endif
