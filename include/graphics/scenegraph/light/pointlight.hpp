#ifndef SG_POINT_LIGHT
#define SG_POINT_LIGHT

#include "graphics/scenegraph/light/light.hpp"
#include <unordered_map>
#include <atomic>

namespace BlueBear::Graphics::SceneGraph::Light {

  class PointLight : public Light {
    glm::vec3 position;
    float constant = 0.0f;
    float linear = 0.0f;
    float quadratic = 0.0f;

  public:
    PointLight(
      glm::vec3 position,
      glm::vec3 ambientComponent,
      glm::vec3 diffuseComponent,
      glm::vec3 specularComponent,
      float constant,
      float linear,
      float quadratic
    );
  };

}

#endif
