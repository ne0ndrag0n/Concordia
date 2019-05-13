#ifndef SG_POINT_LIGHT
#define SG_POINT_LIGHT

#include "graphics/scenegraph/light/light.hpp"
#include "graphics/shader.hpp"
#include <unordered_map>
#include <atomic>

namespace BlueBear::Graphics::SceneGraph::Light {

  class PointLight : public Light {
    static std::atomic< unsigned int > counter;

    int id;

    glm::vec3 position;
    float constant = 0.0f;
    float linear = 0.0f;
    float quadratic = 0.0f;

    struct PointLightUniformBundle {
      Shader::Uniform positionUniform;
      Shader::Uniform constantUniform;
      Shader::Uniform linearUniform;
      Shader::Uniform quadraticUniform;
    };

    std::unordered_map< const void*, PointLightUniformBundle > pointBundles;

  protected:
    void generateUniformBundles( const Shader* shader ) override;
    void send( const Shader& shader ) override;

  public:
    static void sendLightCount( const Shader& shader, const Shader::Uniform uniform );

    PointLight(
      glm::vec3 position,
      glm::vec3 ambientComponent,
      glm::vec3 diffuseComponent,
      glm::vec3 specularComponent,
      float constant,
      float linear,
      float quadratic
    );
    ~PointLight();

    std::string getPreamble() override;
  };

}

#endif
