#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <string>
#include <optional>
#include <glm/glm.hpp>
#include "eventmanager.hpp"

namespace BlueBear {
  namespace Graphics {
    class Shader {
      std::string vPath;
      std::string fPath;

    public:
      using Uniform = GLint;

      GLuint Program;
      static GLint CURRENT_PROGRAM;
      static BasicEvent< void*, const Shader& > SHADER_CHANGE;

      Shader( const GLchar* vertexPath, const GLchar* fragmentPath );
      Shader( const std::string& vertexPath, const std::string& fragmentPath, bool defer = false );

      void sendDeferred();

      std::optional< Uniform > getUniform( const std::string& id ) const;

      void sendData( Uniform uniform, const glm::vec2& value ) const;
      void sendData( Uniform uniform, const glm::vec3& value ) const;
      void sendData( Uniform uniform, const glm::vec4& value ) const;
      void sendData( Uniform uniform, const glm::mat4& value ) const;
      void sendData( Uniform uniform, const int value ) const;
      void sendData( Uniform uniform, const unsigned int value ) const;
      void sendData( Uniform uniform, const float value ) const;

      void use( bool silent = false );
    };
  }
}
#endif
