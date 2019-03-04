#ifndef SG_LIGHT
#define SG_LIGHT

#include "graphics/scenegraph/illuminator.hpp"
#include <glm/glm.hpp>
#include <string>

namespace BlueBear::Graphics::SceneGraph::Light {

    class Light : public Illuminator {
      glm::vec3 ambientComponent;
      glm::vec3 diffuseComponent;
      glm::vec3 specularComponent;

    protected:
      virtual std::string getPreamble() = 0;
      void send() override;

    public:
      Light(
        glm::vec3 ambientComponent,
        glm::vec3 diffuseComponent,
        glm::vec3 specularComponent
      );

      glm::vec3 getAmbient();
      glm::vec3 getDiffuse();
      glm::vec3 getSpecular();

      void setAmbient( glm::vec3 ambientComponent );
      void setDiffuse( glm::vec3 diffuseComponent );
      void setSpecular( glm::vec3 specularComponent );
    };

}

#endif
