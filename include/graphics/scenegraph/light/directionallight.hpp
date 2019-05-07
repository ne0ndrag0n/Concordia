#ifndef SG_DIRECTIONAL_LIGHT
#define SG_DIRECTIONAL_LIGHT

#include "graphics/scenegraph/light/light.hpp"

namespace BlueBear::Graphics { class Shader; }
namespace BlueBear::Graphics::SceneGraph::Light {

  class DirectionalLight : public Light {

    glm::vec3 direction;

  protected:
    std::string getPreamble() override;
    void send( const Shader& shader ) override;

  public:
    DirectionalLight(
      glm::vec3 direction,
      glm::vec3 ambientComponent,
      glm::vec3 diffuseComponent,
      glm::vec3 specularComponent
    );
  };

}

#endif
