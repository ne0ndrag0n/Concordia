#ifndef SG_MATERIAL
#define SG_MATERIAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <memory>

namespace BlueBear {
  namespace Graphics {

    class Texture;

    namespace SceneGraph {

      using TextureList = std::vector< std::shared_ptr< Texture > >;

      class Material {
        glm::vec4 ambientColor;
        glm::vec4 diffuseColor;
        glm::vec4 specularColor;
        TextureList diffuseTextures;
        TextureList specularTextures;
        float shininess;

      public:
        Material( glm::vec4 ambientColor, glm::vec4 diffuseColor, glm::vec4 specularColor, float shininess );
        Material( glm::vec4 ambientColor, TextureList diffuseTextures, TextureList specularTextures, float shininess );

        
      };

    }
  }
}


#endif
