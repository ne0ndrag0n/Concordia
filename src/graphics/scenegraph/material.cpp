#include "graphics/scenegraph/material.hpp"
#include "graphics/texture.hpp"
#include "tools/opengl.hpp"
#include <GL/glew.h>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {

      Material::Material( glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor, float shininess, float opacity ) :
        ambientColor( ambientColor ), diffuseColor( diffuseColor ), specularColor( specularColor ), shininess( shininess ), opacity( opacity ), useAmbient( true ) {}

      Material::Material( glm::vec3 diffuseColor, glm::vec3 specularColor, float shininess, float opacity ) :
        diffuseColor( diffuseColor ), specularColor( specularColor ), shininess( shininess ), opacity( opacity ) {}

      Material::Material( glm::vec3 ambientColor, TextureList diffuseTextures, TextureList specularTextures, float shininess, float opacity ) :
        ambientColor( ambientColor ), diffuseTextures( diffuseTextures ), specularTextures( specularTextures ), shininess( shininess ), opacity( opacity ),
        useAmbient( true ) {
          checkTextureUnits();
        }

      Material::Material( TextureList diffuseTextures, TextureList specularTextures, float shininess, float opacity ) :
        diffuseTextures( diffuseTextures ), specularTextures( specularTextures ), shininess( shininess ), opacity( opacity ) {
          checkTextureUnits();
        }

      Material::Material( glm::vec3 ambientColor, TextureList diffuseTextures, glm::vec3 specularColor, float shininess, float opacity ) :
        ambientColor( ambientColor ), diffuseTextures( diffuseTextures ), specularColor( specularColor ), shininess( shininess ), opacity( opacity ), useAmbient( true ) {
          checkTextureUnits();
        }

      Material::Material( glm::vec3 ambientColor, glm::vec3 diffuseColor, TextureList specularTextures, float shininess, float opacity ) :
        ambientColor( ambientColor ), diffuseColor( diffuseColor ), specularTextures( specularTextures ), shininess( shininess ), opacity( opacity ), useAmbient( true ) {
          checkTextureUnits();
        }

      void Material::checkTextureUnits() {
        unsigned int totalTextures = diffuseTextures.size() + specularTextures.size();

        if( totalTextures > GL_MAX_TEXTURE_UNITS ) {
          throw Material::ExceededTextureUnitsException();
        }
      }

      const glm::vec3& Material::getAmbientColor() const {
        return ambientColor;
      }

      const glm::vec3& Material::getDiffuseColor() const {
        return diffuseColor;
      }

      const glm::vec3& Material::getSpecularColor() const {
        return specularColor;
      }

      const TextureList& Material::getDiffuseTextureList() const {
        return diffuseTextures;
      }

      const TextureList& Material::getSpecularTextureList() const {
        return specularTextures;
      }

      float Material::getShininess() const {
        return shininess;
      }

      float Material::getOpacity() const {
        return opacity;
      }

      void Material::sendDeferredTextures() {
        for( std::shared_ptr< Texture > diffuse : diffuseTextures ) {
          diffuse->sendDeferred();
        }

        for( std::shared_ptr< Texture > specular : specularTextures ) {
          specular->sendDeferred();
        }
      }

      void Material::send() {
        unsigned int counter = 0;

        if( useAmbient ) {
          Tools::OpenGL::setUniform( "material.ambient", ambientColor );
        }

        if( diffuseTextures.empty() ) {
          Tools::OpenGL::setUniform( "material.diffuse", diffuseColor );
        } else {
          for( int i = 0; i != diffuseTextures.size(); i++ ) {
            glActiveTexture( GL_TEXTURE0 + counter );
            glBindTexture( GL_TEXTURE_2D, diffuseTextures[ i ]->id );
            Tools::OpenGL::setUniform( std::string( "material.diffuse" ) + std::to_string( i ), (int) counter++ );
          }
        }

        if( specularTextures.empty() ) {
          Tools::OpenGL::setUniform( "material.specular", specularColor );
        } else {
          for( int i = 0; i != specularTextures.size(); i++ ) {
            glActiveTexture( GL_TEXTURE0 + counter );
            glBindTexture( GL_TEXTURE_2D, specularTextures[ i ]->id );
            Tools::OpenGL::setUniform( std::string( "material.specular" ) + std::to_string( i ), (int) counter++ );

          }
        }

        Tools::OpenGL::setUniform( "material.shininess", shininess );
        Tools::OpenGL::setUniform( "material.opacity", opacity );
      }

    }
  }
}
