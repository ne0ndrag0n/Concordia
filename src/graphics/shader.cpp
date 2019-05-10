#include "graphics/shader.hpp"
#include "tools/opengl.hpp"
#include "eventmanager.hpp"
#include "log.hpp"
#include <regex>
#include <string>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

/**
 * TODO:
 * This code is a mess as it's modified tutorial code from learnopengl.com
 * PLEASE clean this up as it will affect loading times of the application.
 */

namespace BlueBear {
  namespace Graphics {

    GLint Shader::CURRENT_PROGRAM = -1;
    BasicEvent< void*, const Shader& > Shader::SHADER_CHANGE;

    std::string Shader::getFile( const std::string& path ) {
      std::ifstream stream;
      stream.exceptions( std::ifstream::badbit );

      try {
        stream.open( path );

        if( !stream.is_open() ) {
          Log::getInstance().error( "Shader::getFile", "File not found: " + path );
          throw InvalidShaderFileException();
        }

        std::stringstream stringStream;
        stringStream << stream.rdbuf();

        stream.close();

        return stringStream.str();
      } catch( std::ifstream::failure e ) {
        Log::getInstance().error( "Shader::getFile", "Error loading shader file: " + path );
        throw InvalidShaderFileException();
      }
    }

    Shader::FilePackage Shader::getFilePair() {
      return FilePackage{
        preprocess( getFile( vPath ) ),
        preprocess( getFile( fPath ) )
      };
    }

    std::string Shader::preprocess( const std::string& source ) {
      std::string result = source;
      std::regex includeStatement( "#include(?: |\\t)+\"(.+)\"\\n" );

      // Each match will have:
      // 1. the include line itself
      // 2. the captured filename
      std::smatch match;
      while( std::regex_search( result, match, includeStatement ) ) {
        const std::string& filename = match[ 1 ];
        if( visitedFiles.find( filename ) == visitedFiles.end() ) {
          visitedFiles.insert( filename );
          result = match.format( "$`" + preprocess( getFile( filename ) ) +  "$'" );
        } else {
          // Remove match
          result = match.format( "$`$'" );
        }
      }

      return result;
    }

    GLuint Shader::compileVertex( const std::string& source ) {
      GLuint vertex = 0;

      vertex = glCreateShader( GL_VERTEX_SHADER );
      if( !vertex ) {
        Log::getInstance().error( "Shader::compileVertex", "Could not create vertex shader!" );
        throw ShaderCompilationFailure();
      }

      const GLchar* castSource = source.c_str();
      glShaderSource( vertex, 1, &castSource, NULL );

      glCompileShader( vertex );

      GLint success = 0;
      glGetShaderiv( vertex, GL_COMPILE_STATUS, &success );
      if( !success ) {
        GLchar infoLog[ 512 ];
        glGetShaderInfoLog( vertex, 512, NULL, infoLog );
        Log::getInstance().error( "Shader::compileVertex", "Shader compilation failed: " + std::string( infoLog ) );
        throw ShaderCompilationFailure();
      }

      return vertex;
    }

    GLuint Shader::compileFragment( const std::string& source ) {
      GLuint fragment = 0;

      fragment = glCreateShader( GL_FRAGMENT_SHADER );
      if( !fragment ) {
        Log::getInstance().error( "Shader::compileFragment", "Could not create fragment shader!" );
        throw ShaderCompilationFailure();
      }

      const GLchar* castSource = source.c_str();
      glShaderSource( fragment, 1, &castSource, NULL );

      glCompileShader( fragment );

      GLint success = 0;
      glGetShaderiv( fragment, GL_COMPILE_STATUS, &success );
      if( !success ) {
        GLchar infoLog[ 512 ];
        glGetShaderInfoLog( fragment, 512, NULL, infoLog );
        Log::getInstance().error( "Shader::compileFragment", "Shader compilation failed: " + std::string( infoLog ) );
        throw ShaderCompilationFailure();
      }

      return fragment;
    }

    void Shader::sendDeferred() {
      if( vPath.empty() || fPath.empty() ) {
        return;
      }

      FilePackage package = getFilePair();

      GLuint vertex = compileVertex( package.vertex );
      GLuint fragment = compileFragment( package.fragment );

      // Shader Program
      program = glCreateProgram();

      glAttachShader( program, vertex );
      glAttachShader( program, fragment );

      glLinkProgram( program );

      GLint success = 0;
      glGetProgramiv( program, GL_LINK_STATUS, &success );

      if ( !success ) {
          GLchar infoLog[ 512 ];
          glGetProgramInfoLog( program, 512, NULL, infoLog );
          Log::getInstance().error( "Shader::sendDeferred", "Shader linkage failed: " + std::string( infoLog ) );
          throw ShaderLinkFailure();
      }

      glDeleteShader(vertex);
      glDeleteShader(fragment);

      Log::getInstance().debug( "Shader::sendDeferred", vPath + ";" + fPath + " loaded successfully" );
    }

    Shader::Shader( const std::string& vertexPath, const std::string& fragmentPath, bool defer ) : vPath( vertexPath ), fPath( fragmentPath ) {
      if( !defer ) {
        sendDeferred();
      }
    }

    Shader::Shader( const GLchar* vertexPath, const GLchar* fragmentPath ) : vPath( vertexPath ), fPath( fragmentPath ) {
      sendDeferred();
    }

    Shader::Uniform Shader::getUniform( const std::string& id ) const {
      Uniform result = Shader::UNDEFINED_UNIFORM;

      if( program == Shader::CURRENT_PROGRAM ) {
        result = glGetUniformLocation( program, id.c_str() );
      } else {
        Log::getInstance().warn( "Shader::getUniform", "Shader was not set before getUniform was called." );
      }

      return result;
    }

    void Shader::sendData( Uniform uniform, const glm::vec2& value ) const {
      if( program == Shader::CURRENT_PROGRAM ) {
        if( uniform != -1 ) {
          glUniform2f( uniform, value.x, value.y );
        }
      } else {
        Log::getInstance().warn( "Shader::getUniform", "Shader was not set before sendData was called." );
      }
    }

    void Shader::sendData( Uniform uniform, const glm::vec3& value ) const {
      if( program == Shader::CURRENT_PROGRAM ) {
        if( uniform != -1 ) {
          glUniform3f( uniform, value.x, value.y, value.z );
        }
      } else {
        Log::getInstance().warn( "Shader::getUniform", "Shader was not set before sendData was called." );
      }
    }

    void Shader::sendData( Uniform uniform, const glm::vec4& value ) const {
      if( program == Shader::CURRENT_PROGRAM ) {
        if( uniform != -1 ) {
          glUniform4f( uniform, value.x, value.y, value.z, value.w );
        }
      } else {
        Log::getInstance().warn( "Shader::getUniform", "Shader was not set before sendData was called." );
      }
    }

    void Shader::sendData( Uniform uniform, const glm::mat4& value ) const {
      if( program == Shader::CURRENT_PROGRAM ) {
        if( uniform != -1 ) {
          glUniformMatrix4fv( uniform, 1, GL_FALSE, glm::value_ptr( value ) );
        }
      } else {
        Log::getInstance().warn( "Shader::getUniform", "Shader was not set before sendData was called." );
      }
    }

    void Shader::sendData( Uniform uniform, const int value ) const {
      if( program == Shader::CURRENT_PROGRAM ) {
        if( uniform != -1 ) {
          glUniform1i( uniform, value );
        }
      } else {
        Log::getInstance().warn( "Shader::getUniform", "Shader was not set before sendData was called." );
      }
    }

    void Shader::sendData( Uniform uniform, const unsigned int value ) const {
      if( program == Shader::CURRENT_PROGRAM ) {
        if( uniform != -1 ) {
          glUniform1ui( uniform, value );
        }
      } else {
        Log::getInstance().warn( "Shader::getUniform", "Shader was not set before sendData was called." );
      }
    }

    void Shader::sendData( Uniform uniform, const float value ) const {
      if( program == Shader::CURRENT_PROGRAM ) {
        if( uniform != -1 ) {
          glUniform1f( uniform, value );
        }
      } else {
        Log::getInstance().warn( "Shader::getUniform", "Shader was not set before sendData was called." );
      }
    }

    void Shader::sendData( Uniform uniform, unsigned int size, const GLfloat* value ) const {
      if( program == Shader::CURRENT_PROGRAM ) {
        if( uniform != -1 ) {
          glUniformMatrix4fv( uniform, size, GL_FALSE, value );
        }
      } else {
        Log::getInstance().warn( "Shader::getUniform", "Shader was not set before sendData was called." );
      }
    }

    void Shader::use( bool silent ) {
      if( Shader::CURRENT_PROGRAM != program ) {
        glUseProgram( program );
        Shader::CURRENT_PROGRAM = program;
      }

      if( !silent ) {
        SHADER_CHANGE.trigger( *this );
      }
    }

  }
}
