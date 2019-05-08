#include "graphics/scenegraph/material.hpp"
#include "graphics/texture.hpp"
#include "tools/opengl.hpp"
#include "configmanager.hpp"
#include <GL/glew.h>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {

      Material::Material( glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor, float shininess, float opacity ) :
        ambientColor( ambientColor ), diffuseColor( diffuseColor ), specularColor( specularColor ), shininess( shininess ), opacity( opacity ), useAmbient( true ) {
          maxDiffuseTextures = ConfigManager::getInstance().getIntValue( "shader_max_diffuse_textures" );
          maxSpecularTextures = ConfigManager::getInstance().getIntValue( "shader_max_specular_textures" );
        }

      Material::Material( glm::vec3 diffuseColor, glm::vec3 specularColor, float shininess, float opacity ) :
        diffuseColor( diffuseColor ), specularColor( specularColor ), shininess( shininess ), opacity( opacity ) {
          maxDiffuseTextures = ConfigManager::getInstance().getIntValue( "shader_max_diffuse_textures" );
          maxSpecularTextures = ConfigManager::getInstance().getIntValue( "shader_max_specular_textures" );
        }

      Material::Material( glm::vec3 ambientColor, TextureList diffuseTextures, TextureList specularTextures, float shininess, float opacity ) :
        ambientColor( ambientColor ), diffuseTextures( diffuseTextures ), specularTextures( specularTextures ), shininess( shininess ), opacity( opacity ),
        useAmbient( true ) {
          checkTextureUnits();

          maxDiffuseTextures = ConfigManager::getInstance().getIntValue( "shader_max_diffuse_textures" );
          maxSpecularTextures = ConfigManager::getInstance().getIntValue( "shader_max_specular_textures" );
        }

      Material::Material( TextureList diffuseTextures, TextureList specularTextures, float shininess, float opacity ) :
        diffuseTextures( diffuseTextures ), specularTextures( specularTextures ), shininess( shininess ), opacity( opacity ) {
          checkTextureUnits();

          maxDiffuseTextures = ConfigManager::getInstance().getIntValue( "shader_max_diffuse_textures" );
          maxSpecularTextures = ConfigManager::getInstance().getIntValue( "shader_max_specular_textures" );
        }

      Material::Material( glm::vec3 ambientColor, TextureList diffuseTextures, glm::vec3 specularColor, float shininess, float opacity ) :
        ambientColor( ambientColor ), diffuseTextures( diffuseTextures ), specularColor( specularColor ), shininess( shininess ), opacity( opacity ), useAmbient( true ) {
          checkTextureUnits();

          maxDiffuseTextures = ConfigManager::getInstance().getIntValue( "shader_max_diffuse_textures" );
          maxSpecularTextures = ConfigManager::getInstance().getIntValue( "shader_max_specular_textures" );
        }

      Material::Material( glm::vec3 ambientColor, glm::vec3 diffuseColor, TextureList specularTextures, float shininess, float opacity ) :
        ambientColor( ambientColor ), diffuseColor( diffuseColor ), specularTextures( specularTextures ), shininess( shininess ), opacity( opacity ), useAmbient( true ) {
          checkTextureUnits();

          maxDiffuseTextures = ConfigManager::getInstance().getIntValue( "shader_max_diffuse_textures" );
          maxSpecularTextures = ConfigManager::getInstance().getIntValue( "shader_max_specular_textures" );
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

      const Material::MaterialUniforms& Material::getMaterialUniforms( const Shader* shader ) {
        auto it = uniforms.find( shader );
        if( it != uniforms.end() ) {
          return it->second;
        }

        Material::MaterialUniforms& uniform = uniforms[ shader ];
        uniform.ambient = shader->getUniform( "material.ambient" );
        uniform.diffuse = shader->getUniform( "material.diffuse" );
        uniform.specular = shader->getUniform( "material.specular" );

        for( int i = 0; i != maxDiffuseTextures; i++ ) {
          uniform.diffuseArray.emplace_back( shader->getUniform( "material.diffuse" + std::to_string( i ) ) );
        }

        for( int i = 0; i != maxSpecularTextures; i++ ) {
          uniform.specularArray.emplace_back( shader->getUniform( "material.specular" + std::to_string( i ) ) );
        }

        uniform.shininess = shader->getUniform( "material.shininess" );
        uniform.opacity = shader->getUniform( "material.opacity" );

        return uniform;
      }

      void Material::send( const Shader& shader ) {
        const auto& uniform = getMaterialUniforms( &shader );

        if( useAmbient ) {
          shader.sendData( uniform.ambient, ambientColor );
        }

        if( diffuseTextures.empty() ) {
          shader.sendData( uniform.diffuse, diffuseColor );
        } else {
          for( int i = 0; i != diffuseTextures.size() && i < maxDiffuseTextures; i++ ) {
            auto textureUnit = Tools::OpenGL::getTextureUnit();
            if( !textureUnit ) {
              throw Material::TextureUnitUnavailableException();
            }
            lockedTextureUnits.push_back( *textureUnit );

            glActiveTexture( GL_TEXTURE0 + *textureUnit );
            glBindTexture( GL_TEXTURE_2D, diffuseTextures[ i ]->id );
            shader.sendData( uniform.diffuseArray[ i ], (int) *textureUnit );
          }
        }

        if( specularTextures.empty() ) {
          shader.sendData( uniform.specular, specularColor );
        } else {
          for( int i = 0; i != specularTextures.size() && i < maxSpecularTextures; i++ ) {
            auto textureUnit = Tools::OpenGL::getTextureUnit();
            if( !textureUnit ) {
              throw Material::TextureUnitUnavailableException();
            }
            lockedTextureUnits.push_back( *textureUnit );

            glActiveTexture( GL_TEXTURE0 + *textureUnit );
            glBindTexture( GL_TEXTURE_2D, specularTextures[ i ]->id );
            shader.sendData( uniform.specularArray[ i ], (int) *textureUnit );
          }
        }

        shader.sendData( uniform.shininess, shininess );
        shader.sendData( uniform.opacity, opacity );
      }

      void Material::releaseTextureUnits() {
        Tools::OpenGL::returnTextureUnits( lockedTextureUnits );
        lockedTextureUnits.clear();
      }

    }
  }
}
