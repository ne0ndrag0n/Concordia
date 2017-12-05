#ifndef SG_MATERIAL
#define SG_MATERIAL

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <exception>

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
        float shininess = 0.0f;
        bool useAmbient = false;

        void checkTextureUnits();

      public:
        struct ExceededTextureUnitsException : public std::exception {
          const char* what() const throw() {
            return "Exceeded the maximum texture units for this hardware.";
          }
        };

        Material( glm::vec4 ambientColor, glm::vec4 diffuseColor, glm::vec4 specularColor, float shininess );
        Material( glm::vec4 diffuseColor, glm::vec4 specularColor, float shininess );

        Material( glm::vec4 ambientColor, TextureList diffuseTextures, TextureList specularTextures, float shininess );
        Material( TextureList diffuseTextures, TextureList specularTextures, float shininess );

        Material( glm::vec4 ambientColor, TextureList diffuseTextures, float shininess );

        glm::vec4 getAmbientColor();
        glm::vec4 getDiffuseColor();
        glm::vec4 getSpecularColor();

        TextureList getDiffuseTextureList();
        TextureList getSpecularTextureList();

        float getShininess();

        void send();
      };

    }
  }
}


#endif
