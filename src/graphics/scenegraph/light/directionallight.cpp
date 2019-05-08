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

  void DirectionalLight::generateUniformBundles( const Shader* shader ) {
    Light::generateUniformBundles( shader );

    auto it = uniforms.find( shader );
    if( it == uniforms.end() ) {
      std::string preamble = getPreamble() + ".";
      auto& uniform = uniforms[ shader ];

      uniform = shader->getUniform( preamble + "direction" );
    }
  }

  void DirectionalLight::send( const Shader& shader ) {
    Light::send( shader );

    shader.sendData( uniforms[ &shader ], direction );
  }

}
