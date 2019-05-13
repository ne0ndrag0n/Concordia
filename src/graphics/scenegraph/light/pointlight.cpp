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

  std::string PointLight::getPreamble() {
    return "pointLights";
  }

  void PointLight::generateUniformBundles( const Shader* shader ) {
    Light::generateUniformBundles( shader );

    auto it = pointBundles.find( shader );
    if( it == pointBundles.end() ) {
      auto& bundle = pointBundles[ shader ];

      static int maxLights = ConfigManager::getInstance().getIntValue( "shader_max_lights" );
      for( int i = 0; i != maxLights; i++ ) {
        std::string prefix = getPreamble() + "[" + std::to_string( i ) + "].";

        bundle.positionUniform.emplace_back( shader->getUniform( prefix + "position" ) );
        bundle.constantUniform.emplace_back( shader->getUniform( prefix + "constant" ) );
        bundle.linearUniform.emplace_back( shader->getUniform( prefix + "linear" ) );
        bundle.quadraticUniform.emplace_back( shader->getUniform( prefix + "quadratic" ) );
      }

    }
  }

  void PointLight::send( const Shader& shader, unsigned int arrayIndex ) {
    Light::send( shader, arrayIndex );

    const auto& bundle = pointBundles[ &shader ];
    shader.sendData( bundle.positionUniform[ arrayIndex ], position );
    shader.sendData( bundle.constantUniform[ arrayIndex ], constant );
    shader.sendData( bundle.linearUniform[ arrayIndex ], linear );
    shader.sendData( bundle.quadraticUniform[ arrayIndex ], quadratic );
  }

}
