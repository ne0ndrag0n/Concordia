#include "tools/opengl.hpp"
#include "graphics/shader.hpp"

namespace BlueBear {
  namespace Tools {

    std::mutex OpenGL::mutex;

    GLint OpenGL::getUniformID( const std::string& uniform ) {
      return glGetUniformLocation( Graphics::Shader::CURRENT_PROGRAM, uniform.c_str() );
    }

    GLint OpenGL::getUniformLocation( const std::string& uniform ) {
      GLint uniformLocation = OpenGL::getUniformID( uniform );
      if( uniformLocation == -1 ) {
        throw IncorrectShaderException();
      }

      return uniformLocation;
    }

    /**
     * Use this to wrap any OpenGL code that may be run concurrently. OpenGL is completely thread unsafe,
     * and things like loading models while the game loop continues may require locks on anything touching
     * OpenGL.
     */
    void OpenGL::lock( std::function< void() > predicate ) {
      std::lock_guard< std::mutex > guard( OpenGL::mutex );
      predicate();
    }

  }
}
