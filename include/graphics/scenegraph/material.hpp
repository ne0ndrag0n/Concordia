#ifndef SG_MATERIAL
#define SG_MATERIAL

#include "exceptions/genexc.hpp"
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
        glm::vec3 ambientColor;
        glm::vec3 diffuseColor;
        glm::vec3 specularColor;
        TextureList diffuseTextures;
        TextureList specularTextures;
        float shininess = 0.0f;
        bool useAmbient = false;

        void checkTextureUnits();

      public:
        EXCEPTION_TYPE( ExceededTextureUnitsException, "Exceeded the maximum texture units for this hardware." );

        Material( glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor, float shininess );
        Material( glm::vec3 diffuseColor, glm::vec3 specularColor, float shininess );

        Material( glm::vec3 ambientColor, TextureList diffuseTextures, TextureList specularTextures, float shininess );
        Material( TextureList diffuseTextures, TextureList specularTextures, float shininess );

        Material( glm::vec3 ambientColor, TextureList diffuseTextures, glm::vec3 specularColor, float shininess );
        Material( glm::vec3 ambientColor, glm::vec3 diffuseColor, TextureList specularTextures, float shininess );

        virtual ~Material() = default;

        glm::vec3 getAmbientColor();
        glm::vec3 getDiffuseColor();
        glm::vec3 getSpecularColor();

        TextureList getDiffuseTextureList();
        TextureList getSpecularTextureList();

        float getShininess();

        void sendDeferredTextures();

        void send();
      };

    }
  }
}


#endif
