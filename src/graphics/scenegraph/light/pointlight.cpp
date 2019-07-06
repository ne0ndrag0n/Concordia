#include "graphics/scenegraph/light/pointlight.hpp"
#include "tools/opengl.hpp"
#include "configmanager.hpp"

namespace BlueBear::Graphics::SceneGraph::Light {


  PointLight::PointLight( glm::vec3 position, glm::vec3 ambientComponent, glm::vec3 diffuseComponent, glm::vec3 specularComponent, float constant, float linear, float quadratic ) :
    Light::Light( ambientComponent, diffuseComponent, specularComponent ) {
      this->position = position;
      this->constant = constant;
      this->linear = linear;
      this->quadratic = quadratic;
    }

}
