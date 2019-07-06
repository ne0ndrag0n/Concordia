#include "graphics/scenegraph/light/light.hpp"
#include "tools/opengl.hpp"
#include "configmanager.hpp"
#include "eventmanager.hpp"
#include <GL/glew.h>
#include "log.hpp"

namespace BlueBear::Graphics::SceneGraph::Light {

    Light::Light( glm::vec3 ambientComponent, glm::vec3 diffuseComponent, glm::vec3 specularComponent ) :
      ambientComponent( ambientComponent ), diffuseComponent( diffuseComponent ), specularComponent( specularComponent ) {}

    const glm::vec3& Light::getAmbient() const {
      return ambientComponent;
    }

    const glm::vec3& Light::getDiffuse() const {
      return diffuseComponent;
    }

    const glm::vec3& Light::getSpecular() const {
      return specularComponent;
    }

    void Light::setAmbient( glm::vec3 ambientComponent ) {
      this->ambientComponent = ambientComponent;
    }

    void Light::setDiffuse( glm::vec3 diffuseComponent ) {
      this->diffuseComponent = diffuseComponent;
    }

    void Light::setSpecular( glm::vec3 specularComponent ) {
      this->specularComponent = specularComponent;
    }

}
