#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <string>

namespace BlueBear {
  namespace Graphics {
    class Shader {
      std::string vPath;
      std::string fPath;

    public:
      GLuint Program;
      static GLint CURRENT_PROGRAM;

      Shader( const GLchar* vertexPath, const GLchar* fragmentPath );
      Shader( const std::string& vertexPath, const std::string& fragmentPath, bool defer = false );

      void sendDeferred();

      void use();
    };
  }
}
#endif
