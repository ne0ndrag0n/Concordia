#include "graphics/scenegraph/light/pointlight.hpp"
#include "tools/opengl.hpp"

namespace BlueBear::Graphics::SceneGraph::Light {

  std::atomic< unsigned int > PointLight::counter{ 0 };

  PointLight::PointLight( glm::vec3 position, glm::vec3 ambientComponent, glm::vec3 diffuseComponent, glm::vec3 specularComponent, float constant, float linear, float quadratic ) :
    Light::Light( ambientComponent, diffuseComponent, specularComponent ) {
      this->position = position;
      this->constant = constant;
      this->linear = linear;
      this->quadratic = quadratic;

      id = counter.load();
      counter++;
    }

  PointLight::~PointLight() {
    counter--;
  }

  void PointLight::sendLightCount() {
    Tools::OpenGL::setUniform( "numPointLights", counter.load() );
  }

  std::string PointLight::getPreamble() {
    return "pointLights[" + std::to_string( id ) + "]";
  }

  void PointLight::send( const Shader& shader ) {
    Light::send( shader );

    std::string preamble = getPreamble() + ".";
    Tools::OpenGL::setUniform( preamble + "position", position );
    Tools::OpenGL::setUniform( preamble + "constant", constant );
    Tools::OpenGL::setUniform( preamble + "linear", linear );
    Tools::OpenGL::setUniform( preamble + "quadratic", quadratic );
  }

}
