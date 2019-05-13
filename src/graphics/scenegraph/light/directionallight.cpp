#include "graphics/scenegraph/light/directionallight.hpp"
#include "tools/opengl.hpp"
#include "configmanager.hpp"

namespace BlueBear::Graphics::SceneGraph::Light {

  DirectionalLight::DirectionalLight( glm::vec3 direction, glm::vec3 ambientComponent, glm::vec3 diffuseComponent, glm::vec3 specularComponent ) :
    Light::Light( ambientComponent, diffuseComponent, specularComponent ) {
      this->direction = direction;
    }

  std::string DirectionalLight::getPreamble() {
    return "directionalLights";
  }

  void DirectionalLight::generateUniformBundles( const Shader* shader ) {
    Light::generateUniformBundles( shader );

    auto it = uniforms.find( shader );
    if( it == uniforms.end() ) {
      auto& uniform = uniforms[ shader ];

      static int maxLights = ConfigManager::getInstance().getIntValue( "shader_max_lights" );
      for( int i = 0; i != maxLights; i++ ) {
        uniform.emplace_back( shader->getUniform( getPreamble() + "[" + std::to_string( i ) + "].direction" ) );
      }
    }
  }

  void DirectionalLight::send( const Shader& shader, unsigned int arrayIndex ) {
    Light::send( shader, arrayIndex );

    shader.sendData( uniforms[ &shader ][ arrayIndex ], direction );
  }

  glm::vec3 DirectionalLight::getDirection() const {
    return direction;
  }

}
