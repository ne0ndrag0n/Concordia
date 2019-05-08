#ifndef SG_DIRECTIONAL_LIGHT
#define SG_DIRECTIONAL_LIGHT

#include "graphics/scenegraph/light/light.hpp"
#include "graphics/shader.hpp"
#include <unordered_map>

namespace BlueBear::Graphics::SceneGraph::Light {

  class DirectionalLight : public Light {

    glm::vec3 direction;

    std::unordered_map< const void*, Shader::Uniform > uniforms;

  protected:
    std::string getPreamble() override;
    void generateUniformBundles( const Shader* shader ) override;
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
