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

    void Light::generateUniformBundles( const Shader* shader ) {
      auto it = bundles.find( shader );
      if( it == bundles.end() ) {
        std::string preamble = getPreamble() + ".";
        auto& bundle = bundles[ shader ];

        bundle.ambientUniform = shader->getUniform( preamble + "ambient" );
        bundle.diffuseUniform = shader->getUniform( preamble + "diffuse" );
        bundle.specularUniform = shader->getUniform( preamble + "specular" );
      }
    }

    /**
     * All light locations can be sent before drawing all potentially lit components
     */
    void Light::send( const Shader& shader ) {
      generateUniformBundles( &shader );
      const auto& bundle = bundles[ &shader ];

      shader.sendData( bundle.ambientUniform, ambientComponent );
      shader.sendData( bundle.diffuseUniform, diffuseComponent );
      shader.sendData( bundle.specularUniform, specularComponent );
    }

}
