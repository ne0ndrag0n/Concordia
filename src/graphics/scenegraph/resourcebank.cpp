#include "graphics/scenegraph/resourcebank.hpp"
#include "graphics/texture.hpp"
#include "graphics/shader.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {

      bool ResourceBank::listsCongruent( const TextureList& list1, const TextureList& list2 ) {
        if( list1.size() == list2.size() ) {
          for( int i = 0; i != list1.size(); i++ ) {
            if( list1.at( i ) != list2.at( i ) ) {
              return false;
            }
          }

          return true;
        }

        return false;
      }

      std::shared_ptr< Shader > ResourceBank::getOrCreateShader( const std::string& vertexPath, const std::string& fragmentPath, bool defer ) {
        std::string key = vertexPath + ":" + fragmentPath;

        std::lock_guard< std::mutex > lock( shadersMutex );
        auto it = shaders.find( key );
        if( it != shaders.end() ) {
          return it->second;
        } else {
          return shaders[ key ] = std::make_shared< Shader >( vertexPath, fragmentPath, defer );
        }
      }

      std::shared_ptr< Texture > ResourceBank::getOrCreateTexture( const std::string& path, bool defer ) {
        std::lock_guard< std::mutex > lock( texturesMutex );
        auto it = textures.find( path );
        if( it != textures.end() ) {
          return it->second;
        } else {
          return textures[ path ] = std::make_shared< Texture >( path, defer );
        }
      }

      std::shared_ptr< Material > ResourceBank::getOrCreateMaterial( const glm::vec3& ambient, const TextureList& diffuse, const TextureList& specular, float shininess, float opacity ) {
        for( auto it = materials.begin(); it != materials.end(); ++it ) {
          std::shared_ptr< Material > sample;
          {
            std::lock_guard< std::mutex > lock( materialsMutex );
            sample = *it;
          }

          if( sample->getShininess() == shininess && sample->getOpacity() == opacity && sample->getAmbientColor() == ambient ) {
            if( listsCongruent( diffuse, sample->getDiffuseTextureList() ) && listsCongruent( specular, sample->getSpecularTextureList() ) ) {
              return sample;
            }
          }
        }

        std::shared_ptr< Material > result = std::make_shared< Material >( ambient, diffuse, specular, shininess, opacity );
        {
          std::lock_guard< std::mutex > lock( materialsMutex );
          materials.push_back( result );
        }
        return result;
      }

      std::shared_ptr< Material > ResourceBank::getOrCreateMaterial( const glm::vec3& ambient, const TextureList& diffuse, const glm::vec3& specular, float shininess, float opacity ) {
        for( auto it = materials.begin(); it != materials.end(); ++it ) {
          std::shared_ptr< Material > sample;
          {
            std::lock_guard< std::mutex > lock( materialsMutex );
            sample = *it;
          }

          if( sample->getShininess() == shininess && sample->getOpacity() == opacity && sample->getAmbientColor() == ambient && sample->getSpecularColor() == specular ) {
            if( listsCongruent( diffuse, sample->getDiffuseTextureList() ) ) {
              return sample;
            }
          }
        }

        std::shared_ptr< Material > result = std::make_shared< Material >( ambient, diffuse, specular, shininess, opacity );
        {
          std::lock_guard< std::mutex > lock( materialsMutex );
          materials.push_back( result );
        }
        return result;
      }

      std::shared_ptr< Material > ResourceBank::getOrCreateMaterial( const glm::vec3& ambient, const glm::vec3& diffuse, const TextureList& specular, float shininess, float opacity ) {
        for( auto it = materials.begin(); it != materials.end(); ++it ) {
          std::shared_ptr< Material > sample;
          {
            std::lock_guard< std::mutex > lock( materialsMutex );
            sample = *it;
          }

          if( sample->getShininess() == shininess && sample->getOpacity() == opacity && sample->getAmbientColor() == ambient && sample->getDiffuseColor() == diffuse ) {
            if( listsCongruent( specular, sample->getSpecularTextureList() ) ) {
              return sample;
            }
          }
        }

        std::shared_ptr< Material > result = std::make_shared< Material >( ambient, diffuse, specular, shininess, opacity );
        {
          std::lock_guard< std::mutex > lock( materialsMutex );
          materials.push_back( result );
        }
        return result;
      }

      std::shared_ptr< Material > ResourceBank::getOrCreateMaterial( const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float shininess, float opacity ) {
        for( auto it = materials.begin(); it != materials.end(); ++it ) {
          std::shared_ptr< Material > sample;
          {
            std::lock_guard< std::mutex > lock( materialsMutex );
            sample = *it;
          }

          if( sample->getShininess() == shininess && sample->getOpacity() == opacity && sample->getAmbientColor() == ambient && sample->getDiffuseColor() == diffuse && sample->getSpecularColor() == specular ) {
            return sample;
          }
        }

        std::shared_ptr< Material > result = std::make_shared< Material >( ambient, diffuse, specular, shininess, opacity );
        {
          std::lock_guard< std::mutex > lock( materialsMutex );
          materials.push_back( result );
        }
        return result;
      }

    }
  }
}
