#include "graphics/scenegraph/light/light.hpp"
#include "tools/opengl.hpp"
#include "eventmanager.hpp"
#include <GL/glew.h>
#include "log.hpp"

namespace BlueBear::Graphics::SceneGraph::Light {

    Light::Light( glm::vec3 ambientComponent, glm::vec3 diffuseComponent, glm::vec3 specularComponent ) :
      ambientComponent( ambientComponent ), diffuseComponent( diffuseComponent ), specularComponent( specularComponent ) {}

    glm::vec3 Light::getAmbient() {
      return ambientComponent;
    }

    glm::vec3 Light::getDiffuse() {
      return diffuseComponent;
    }

    glm::vec3 Light::getSpecular() {
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

    /**
     * All light locations can be sent before drawing all potentially lit components
     */
    void Light::send() {
      std::string preamble = getPreamble() + ".";

      Tools::OpenGL::setUniform( preamble + "ambient", ambientComponent );
      Tools::OpenGL::setUniform( preamble + "diffuse", diffuseComponent );
      Tools::OpenGL::setUniform( preamble + "specular", specularComponent );
    }

}
