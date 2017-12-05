#include "graphics/scenegraph/material.hpp"
#include "graphics/texture.hpp"
#include "tools/opengl.hpp"
#include <GL/glew.h>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {

      Material::Material( glm::vec4 ambientColor, glm::vec4 diffuseColor, glm::vec4 specularColor, float shininess ) :
        ambientColor( ambientColor ), diffuseColor( diffuseColor ), specularColor( specularColor ), shininess( shininess ), useAmbient( true ) {}

      Material::Material( glm::vec4 diffuseColor, glm::vec4 specularColor, float shininess ) :
        diffuseColor( diffuseColor ), specularColor( specularColor ), shininess( shininess ) {}

      Material::Material( glm::vec4 ambientColor, TextureList diffuseTextures, TextureList specularTextures, float shininess ) :
        ambientColor( ambientColor ), diffuseTextures( diffuseTextures ), specularTextures( specularTextures ), shininess( shininess ),
        useAmbient( true ) {
          checkTextureUnits();
        }

      Material::Material( TextureList diffuseTextures, TextureList specularTextures, float shininess ) :
        diffuseTextures( diffuseTextures ), specularTextures( specularTextures ), shininess( shininess ) {
          checkTextureUnits();
        }

      Material::Material( glm::vec4 ambientColor, TextureList diffuseTextures, float shininess ) :
        ambientColor( ambientColor ), diffuseTextures( diffuseTextures ), shininess( shininess ), useAmbient( true ) {
          checkTextureUnits();
        }

      void Material::checkTextureUnits() {
        unsigned int totalTextures = diffuseTextures.size() + specularTextures.size();

        if( totalTextures > GL_MAX_TEXTURE_UNITS ) {
          throw Material::ExceededTextureUnitsException();
        }
      }

      glm::vec4 Material::getAmbientColor() {
        return ambientColor;
      }

      glm::vec4 Material::getDiffuseColor() {
        return diffuseColor;
      }

      glm::vec4 Material::getSpecularColor() {
        return specularColor;
      }

      TextureList Material::getDiffuseTextureList() {
        return diffuseTextures;
      }

      TextureList Material::getSpecularTextureList() {
        return specularTextures;
      }

      float Material::getShininess() {
        return shininess;
      }

      void Material::send() {
        unsigned int counter = 0;

        if( useAmbient ) {
          glUniform4f(
            Tools::OpenGL::getUniformLocation( "material.ambient" ),
            ambientColor[ 0 ], ambientColor[ 1 ], ambientColor[ 2 ], ambientColor[ 3 ]
          );
        }

        if( diffuseTextures.empty() ) {
          glUniform4f(
            Tools::OpenGL::getUniformLocation( "material.diffuse" ),
            diffuseColor[ 0 ], diffuseColor[ 1 ], diffuseColor[ 2 ], diffuseColor[ 3 ]
          );
        } else {
          for( int i = 0; i != diffuseTextures.size(); i++ ) {
            glActiveTexture( GL_TEXTURE0 + counter++ );
            glBindTexture( GL_TEXTURE_2D, diffuseTextures[ i ]->id );
            glUniform1i( Tools::OpenGL::getUniformLocation( std::string( "material.diffuse" ) + std::to_string( i ) ), i );
          }
        }

        if( specularTextures.empty() ) {
          glUniform4f(
            Tools::OpenGL::getUniformLocation( "material.specular" ),
            specularColor[ 0 ], specularColor[ 1 ], specularColor[ 2 ], specularColor[ 3 ]
          );
        } else {
          for( int i = 0; i != specularTextures.size(); i++ ) {
            glActiveTexture( GL_TEXTURE0 + counter++ );
            glBindTexture( GL_TEXTURE_2D, specularTextures[ i ]->id );
            glUniform1i( Tools::OpenGL::getUniformLocation( std::string( "material.specular" ) + std::to_string( i ) ), i );
          }
        }

        glUniform1f( Tools::OpenGL::getUniformLocation( "material.shininess" ), shininess );
      }

    }
  }
}
