#ifndef SHADER_H
#define SHADER_H

#include "exceptions/genexc.hpp"
#include "eventmanager.hpp"
#include <GL/glew.h>
#include <string>
#include <glm/glm.hpp>
#include <unordered_set>

namespace BlueBear {
  namespace Graphics {
    class Shader {
      std::string vPath;
      std::string fPath;
      std::unordered_set< std::string > visitedFiles;

      struct FilePackage {
        std::string vertex;
        std::string fragment;
      };

      std::string getFile( const std::string& path );
      FilePackage getFilePair();
      std::string preprocess( const std::string& source );

      GLuint compileVertex( const std::string& source );
      GLuint compileFragment( const std::string& source );

    public:
      EXCEPTION_TYPE( InvalidShaderFileException, "Could not open shader file" );
      EXCEPTION_TYPE( ShaderCompilationFailure, "Failed to compile shader" );
      EXCEPTION_TYPE( ShaderLinkFailure, "Failed to link shader" );

      using Uniform = GLint;
      static constexpr const Uniform UNDEFINED_UNIFORM = -1;

      GLuint program;
      static GLint CURRENT_PROGRAM;
      static BasicEvent< void*, const Shader& > SHADER_CHANGE;

      Shader( const GLchar* vertexPath, const GLchar* fragmentPath );
      Shader( const std::string& vertexPath, const std::string& fragmentPath, bool defer = false );

      void sendDeferred();

      Uniform getUniform( const std::string& id ) const;

      void sendData( Uniform uniform, const glm::vec2& value ) const;
      void sendData( Uniform uniform, const glm::vec3& value ) const;
      void sendData( Uniform uniform, const glm::vec4& value ) const;
      void sendData( Uniform uniform, const glm::mat4& value ) const;
      void sendData( Uniform uniform, const int value ) const;
      void sendData( Uniform uniform, const unsigned int value ) const;
      void sendData( Uniform uniform, const float value ) const;
      void sendData( Uniform uniform, unsigned int size, const GLfloat* value ) const;

      void use( bool silent = false );
    };
  }
}
#endif
