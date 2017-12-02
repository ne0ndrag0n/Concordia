#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>

namespace BlueBear {
  namespace Graphics {
    class Shader {
      public:
          GLuint Program;
          static GLint CURRENT_PROGRAM;
          Shader( const GLchar* vertexPath, const GLchar* fragmentPath );
          void use();
    };
  }
}
#endif
