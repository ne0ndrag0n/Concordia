#ifndef SG_LIGHT
#define SG_LIGHT

#include <glm/glm.hpp>
#include <unordered_map>
#include <string>

namespace BlueBear::Graphics::SceneGraph::Light {

    class Light {
      glm::vec3 ambientComponent;
      glm::vec3 diffuseComponent;
      glm::vec3 specularComponent;

    public:
      Light(
        glm::vec3 ambientComponent,
        glm::vec3 diffuseComponent,
        glm::vec3 specularComponent
      );

      const glm::vec3& getAmbient() const;
      const glm::vec3& getDiffuse() const;
      const glm::vec3& getSpecular() const;

      void setAmbient( glm::vec3 ambientComponent );
      void setDiffuse( glm::vec3 diffuseComponent );
      void setSpecular( glm::vec3 specularComponent );
    };

}

#endif
