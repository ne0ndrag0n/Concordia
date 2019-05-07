#ifndef SG_POINT_LIGHT
#define SG_POINT_LIGHT

#include "graphics/scenegraph/light/light.hpp"
#include <atomic>

namespace BlueBear::Graphics { class Shader; }
namespace BlueBear::Graphics::SceneGraph::Light {

  class PointLight : public Light {
    static std::atomic< unsigned int > counter;

    int id;
    glm::vec3 position;
    float constant = 0.0f;
    float linear = 0.0f;
    float quadratic = 0.0f;

  protected:
    std::string getPreamble() override;
    void send( const Shader& shader ) override;

  public:
    static void sendLightCount();

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
  };

}

#endif
