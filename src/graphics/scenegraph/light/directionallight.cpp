#include "graphics/scenegraph/light/directionallight.hpp"
#include "tools/opengl.hpp"

namespace BlueBear::Graphics::SceneGraph::Light {

  DirectionalLight::DirectionalLight( glm::vec3 direction, glm::vec3 ambientComponent, glm::vec3 diffuseComponent, glm::vec3 specularComponent ) :
    Light::Light( ambientComponent, diffuseComponent, specularComponent ) {
      this->direction = direction;
    }

  std::string DirectionalLight::getPreamble() {
    return "directionalLight";
  }

  void DirectionalLight::send( const Shader& shader ) {
    Light::send( shader );

    std::string preamble = getPreamble() + ".";
    Tools::OpenGL::setUniform( preamble + "direction", direction );
  }

}
