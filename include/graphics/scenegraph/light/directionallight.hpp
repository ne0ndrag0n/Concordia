#ifndef SG_DIRECTIONAL_LIGHT
#define SG_DIRECTIONAL_LIGHT

#include "graphics/scenegraph/light/light.hpp"
#include <unordered_map>

namespace BlueBear::Graphics::SceneGraph::Light {

  class DirectionalLight : public Light {
    glm::vec3 direction;

  public:
    DirectionalLight(
      glm::vec3 direction,
      glm::vec3 ambientComponent,
      glm::vec3 diffuseComponent,
      glm::vec3 specularComponent
    );

    glm::vec3 getDirection() const;
  };

}

#endif
