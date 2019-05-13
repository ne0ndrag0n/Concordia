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

    void Light::generateUniformBundles( const Shader* shader ) {
      auto it = bundles.find( shader );
      if( it == bundles.end() ) {
        auto& bundle = bundles[ shader ];

        static int maxLights = ConfigManager::getInstance().getIntValue( "shader_max_lights" );
        for( int i = 0; i != maxLights; i++ ) {
          std::string prefix = getPreamble() + "[" + std::to_string( i ) + "].";

          bundle.ambientUniform.emplace_back( shader->getUniform( prefix + "ambient" ) );
          bundle.diffuseUniform.emplace_back( shader->getUniform( prefix + "diffuse" ) );
          bundle.specularUniform.emplace_back( shader->getUniform( prefix + "specular" ) );
        }
      }
    }

    /**
     * All light locations can be sent before drawing all potentially lit components
     */
    void Light::send( const Shader& shader, unsigned int arrayIndex ) {
      generateUniformBundles( &shader );
      const auto& bundle = bundles[ &shader ];

      shader.sendData( bundle.ambientUniform[ arrayIndex ], ambientComponent );
      shader.sendData( bundle.diffuseUniform[ arrayIndex ], diffuseComponent );
      shader.sendData( bundle.specularUniform[ arrayIndex ], specularComponent );
    }

}
