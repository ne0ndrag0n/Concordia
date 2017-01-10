#include "graphics/material.hpp"
#include "graphics/texture.hpp"
#include "tools/opengl.hpp"
#include <GL/glew.h>
#include <string>
#include <sstream>

namespace BlueBear {
  namespace Graphics {

    Material::Material() {}
    Material::Material( TextureList diffuseTextures ) : diffuseTextures( diffuseTextures ) {}
    Material::Material( TextureList diffuseTextures, TextureList specularTextures ) : diffuseTextures( diffuseTextures ), specularTextures( specularTextures ) {}
    Material::Material( const std::shared_ptr< Texture >& diffuseTexture ) {
      diffuseTextures.push_back( diffuseTexture );
    }

    void Material::sendToShader() {
      auto numTextures = diffuseTextures.size();

      for( int i = 0; i != numTextures; i++ ) {
        glActiveTexture( GL_TEXTURE0 + i );
          std::stringstream stream;
          stream << "diffuse" << i;
          std::string uniformName = stream.str();
          glBindTexture( GL_TEXTURE_2D, diffuseTextures[ i ]->id );
          glUniform1i( Tools::OpenGL::getUniformLocation( uniformName ), i );
      }
    }

  }
}
