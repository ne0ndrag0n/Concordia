#ifndef BB_RESOURCE_BANK
#define BB_RESOURCE_BANK

#include "graphics/scenegraph/material.hpp"
#include <tbb/concurrent_vector.h>
#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>
#include <mutex>

namespace BlueBear {
  namespace Graphics {
    class Texture;
    class Shader;

    namespace Utilities {
      class ShaderManager;
    }

    namespace SceneGraph {

      /**
       * Facilitates reuse of common by-ref objects used in Models
       */
      class ResourceBank {
        Utilities::ShaderManager& shaderManager;
        tbb::concurrent_vector< std::shared_ptr< Material > > materials;
        std::unordered_map< std::string, std::shared_ptr< Texture > > textures;
        std::mutex materialsMutex;
        std::mutex texturesMutex;
        std::mutex shadersMutex;

        bool listsCongruent( const TextureList& list1, const TextureList& list2 );

      public:
        ResourceBank( Utilities::ShaderManager& shaderManager );

        std::shared_ptr< Shader > getOrCreateShader( const std::string& vertexPath, const std::string& fragmentPath, bool defer );

        std::shared_ptr< Texture > getOrCreateTexture( const std::string& path, bool defer );

        std::shared_ptr< Material > getOrCreateMaterial( const glm::vec3& ambient, const TextureList& diffuse, const TextureList& specular, float shininess, float opacity );
        std::shared_ptr< Material > getOrCreateMaterial( const glm::vec3& ambient, const TextureList& diffuse, const glm::vec3& specular, float shininess, float opacity );
        std::shared_ptr< Material > getOrCreateMaterial( const glm::vec3& ambient, const glm::vec3& diffuse, const TextureList& specular, float shininess, float opacity );
        std::shared_ptr< Material > getOrCreateMaterial( const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float shininess, float opacity );
      };

    }
  }
}

#endif
