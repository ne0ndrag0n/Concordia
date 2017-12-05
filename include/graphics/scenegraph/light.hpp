#ifndef SG_LIGHT
#define SG_LIGHT

#include <glm/glm.hpp>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {

      class Light {
        glm::vec3 position;
        glm::vec3 ambientComponent;
        glm::vec3 diffuseComponent;
        glm::vec3 specularComponent;

      public:
        Light(
          glm::vec3 position,
          glm::vec3 ambientComponent,
          glm::vec3 diffuseComponent,
          glm::vec3 specularComponent
        );

        glm::vec3 getPosition();
        glm::vec3 getAmbient();
        glm::vec3 getDiffuse();
        glm::vec3 getSpecular();

        void setPosition( glm::vec3 position );
        void setAmbient( glm::vec3 ambientComponent );
        void setDiffuse( glm::vec3 diffuseComponent );
        void setSpecular( glm::vec3 specularComponent );

        void send();
      };

    }
  }
}

#endif
