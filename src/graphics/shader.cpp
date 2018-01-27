#include "graphics/shader.hpp"
#include "tools/opengl.hpp"
#include "eventmanager.hpp"
#include "log.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <GL/glew.h>

/**
 * TODO:
 * This code is a mess as it's modified tutorial code from learnopengl.com
 * PLEASE clean this up as it will affect loading times of the application.
 */

namespace BlueBear {
  namespace Graphics {

    GLint Shader::CURRENT_PROGRAM = -1;

    void Shader::sendDeferred() {
      if( vPath.empty() && fPath.empty() ) {
        return;
      }

      const char* vertexPath = vPath.c_str();
      const char* fragmentPath = fPath.c_str();

      // 1. Retrieve the vertex/fragment source code from filePath
      std::string vertexCode;
      std::string fragmentCode;
      std::ifstream vShaderFile;
      std::ifstream fShaderFile;
      // ensures ifstream objects can throw exceptions:
      vShaderFile.exceptions (std::ifstream::badbit);
      fShaderFile.exceptions (std::ifstream::badbit);
      try
      {
          // Open files
          vShaderFile.open(vertexPath);
          fShaderFile.open(fragmentPath);
          std::stringstream vShaderStream, fShaderStream;
          // Read file's buffer contents into streams
          vShaderStream << vShaderFile.rdbuf();
          fShaderStream << fShaderFile.rdbuf();
          // close file handlers
          vShaderFile.close();
          fShaderFile.close();
          // Convert stream into string
          vertexCode = vShaderStream.str();
          fragmentCode = fShaderStream.str();
      }
      catch (std::ifstream::failure e) {
        Log::getInstance().error( "Shader::Shader", "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" );
      }
      const GLchar* vShaderCode = vertexCode.c_str();
      const GLchar * fShaderCode = fragmentCode.c_str();
      // 2. Compile shaders
      GLuint vertex = 0;
      GLuint fragment = 0;
      GLint success = 0;
      GLchar infoLog[512];
      // Vertex Shader
      vertex = glCreateShader(GL_VERTEX_SHADER);
      if( !vertex ) {
        Log::getInstance().error( "Shader::Shader", "Could not create shader object!" );
        return;
      }
      glShaderSource(vertex, 1, &vShaderCode, NULL);
      glCompileShader(vertex);
      // Print compile errors if any
      glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
      if (!success)
      {
          glGetShaderInfoLog(vertex, 512, NULL, infoLog);
          std::stringstream stream;
          stream << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog;
          Log::getInstance().error( "Shader::Shader", stream.str() );
      }
      // Fragment Shader
      fragment = glCreateShader(GL_FRAGMENT_SHADER);
      glShaderSource(fragment, 1, &fShaderCode, NULL);
      glCompileShader(fragment);
      // Print compile errors if any
      glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
      if (!success)
      {
          glGetShaderInfoLog(fragment, 512, NULL, infoLog);
          std::stringstream stream;
          stream << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog;
          Log::getInstance().error( "Shader::Shader", stream.str() );
      }
      // Shader Program
      this->Program = glCreateProgram();
      glAttachShader(this->Program, vertex);
      glAttachShader(this->Program, fragment);
      glLinkProgram(this->Program);
      // Print linking errors if any
      glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
      if (!success)
      {
          glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
          std::stringstream stream;
          stream << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog;
          Log::getInstance().error( "Shader::Shader", stream.str() );
      }
      // Delete the shaders as they're linked into our program now and no longer necessery
      glDeleteShader(vertex);
      glDeleteShader(fragment);

      Log::getInstance().debug( "Shader::Shader", vPath + ";" + fPath + " loaded successfully" );

      vPath.clear();
      fPath.clear();
    }

    Shader::Shader( const std::string& vertexPath, const std::string& fragmentPath, bool defer ) : vPath( vertexPath ), fPath( fragmentPath ) {
      if( !defer ) {
        sendDeferred();
      }
    }

    Shader::Shader( const GLchar* vertexPath, const GLchar* fragmentPath ) : vPath( vertexPath ), fPath( fragmentPath ) {
      sendDeferred();
    }

    void Shader::use() {
      if( Shader::CURRENT_PROGRAM != this->Program ) {
        glUseProgram( this->Program );
        Shader::CURRENT_PROGRAM = this->Program;
      }

      eventManager.SHADER_CHANGE.trigger();
    }

  }
}
