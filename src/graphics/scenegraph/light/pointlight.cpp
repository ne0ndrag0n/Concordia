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

  void PointLight::generateUniformBundles( const Shader* shader ) {
    Light::generateUniformBundles( shader );

    auto it = pointBundles.find( shader );
    if( it == pointBundles.end() ) {
      std::string preamble = getPreamble() + ".";
      auto& bundle = pointBundles[ shader ];

      bundle.positionUniform = shader->getUniform( preamble + "position" );
      bundle.constantUniform = shader->getUniform( preamble + "constant" );
      bundle.linearUniform   = shader->getUniform( preamble + "linear" );
      bundle.quadraticUniform = shader->getUniform( preamble + "quadratic" );
    }
  }

  void PointLight::send( const Shader& shader ) {
    Light::send( shader );

    const auto& bundle = pointBundles[ &shader ];
    shader.sendData( bundle.positionUniform, position );
    shader.sendData( bundle.constantUniform, constant );
    shader.sendData( bundle.linearUniform, linear );
    shader.sendData( bundle.quadraticUniform, quadratic );
  }

}
