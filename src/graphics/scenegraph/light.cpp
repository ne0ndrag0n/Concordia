#include "graphics/scenegraph/light.hpp"
#include "tools/opengl.hpp"
#include <GL/glew.h>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {

      Light::Light( glm::vec3 position, glm::vec3 ambientComponent, glm::vec3 diffuseComponent, glm::vec3 specularComponent ) :
        position( position ), ambientComponent( ambientComponent ), diffuseComponent( diffuseComponent ), specularComponent( specularComponent ) {}

      glm::vec3 Light::getPosition() {
        return position;
      }

      glm::vec3 Light::getAmbient() {
        return ambientComponent;
      }

      glm::vec3 Light::getDiffuse() {
        return diffuseComponent;
      }

      glm::vec3 Light::getSpecular() {
        return specularComponent;
      }

      void Light::setPosition( glm::vec3 position ) {
        this->position = position;
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
        glUniform3f(
          Tools::OpenGL::getUniformLocation( "light.position" ),
          position[ 0 ], position[ 1 ], position[ 2 ]
        );

        glUniform3f(
          Tools::OpenGL::getUniformLocation( "light.ambient" ),
          ambientComponent[ 0 ], ambientComponent[ 1 ], ambientComponent[ 2 ]
        );

        glUniform3f(
          Tools::OpenGL::getUniformLocation( "light.diffuse" ),
          diffuseComponent[ 0 ], diffuseComponent[ 1 ], diffuseComponent[ 2 ]
        );

        glUniform3f(
          Tools::OpenGL::getUniformLocation( "light.specular" ),
          specularComponent[ 0 ], specularComponent[ 1 ], specularComponent[ 2 ]
        );
      }

    }
  }
}
