#include "graphics/scenegraph/light/directionallight.hpp"
#include "tools/opengl.hpp"
#include "configmanager.hpp"

namespace BlueBear::Graphics::SceneGraph::Light {

  DirectionalLight::DirectionalLight( glm::vec3 direction, glm::vec3 ambientComponent, glm::vec3 diffuseComponent, glm::vec3 specularComponent ) :
    Light::Light( ambientComponent, diffuseComponent, specularComponent ) {
      this->direction = direction;
    }

  glm::vec3 DirectionalLight::getDirection() const {
    return direction;
  }

}
