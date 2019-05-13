#ifndef SG_POINT_LIGHT
#define SG_POINT_LIGHT

#include "graphics/scenegraph/light/light.hpp"
#include "graphics/shader.hpp"
#include <unordered_map>
#include <atomic>

namespace BlueBear::Graphics::SceneGraph::Light {

  class PointLight : public Light {

    glm::vec3 position;
    float constant = 0.0f;
    float linear = 0.0f;
    float quadratic = 0.0f;

    struct PointLightUniformBundle {
      std::vector< Shader::Uniform > positionUniform;
      std::vector< Shader::Uniform > constantUniform;
      std::vector< Shader::Uniform > linearUniform;
      std::vector< Shader::Uniform > quadraticUniform;
    };

    std::unordered_map< const void*, PointLightUniformBundle > pointBundles;

  protected:
    void generateUniformBundles( const Shader* shader ) override;
    void send( const Shader& shader, unsigned int arrayIndex ) override;

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

    std::string getPreamble() override;
  };

}

#endif
