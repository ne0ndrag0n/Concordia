#ifndef SG_LIGHT
#define SG_LIGHT

#include <glm/glm.hpp>
#include <atomic>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {

      class Light {
        int id = -1;
        glm::vec3 position;
        glm::vec3 ambientComponent;
        glm::vec3 diffuseComponent;
        glm::vec3 specularComponent;

        static std::atomic< unsigned int > counter;

        void send();

      public:
        Light(
          glm::vec3 position,
          glm::vec3 ambientComponent,
          glm::vec3 diffuseComponent,
          glm::vec3 specularComponent
        );

        ~Light();

        glm::vec3 getPosition();
        glm::vec3 getAmbient();
        glm::vec3 getDiffuse();
        glm::vec3 getSpecular();

        void setPosition( glm::vec3 position );
        void setAmbient( glm::vec3 ambientComponent );
        void setDiffuse( glm::vec3 diffuseComponent );
        void setSpecular( glm::vec3 specularComponent );
      };

    }
  }
}

#endif
