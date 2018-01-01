#include "graphics/scenegraph/light.hpp"
#include "tools/opengl.hpp"
#include "eventmanager.hpp"
#include <GL/glew.h>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {

      std::atomic< unsigned int > Light::counter{ 0 };

      Light::Light( glm::vec3 position, glm::vec3 ambientComponent, glm::vec3 diffuseComponent, glm::vec3 specularComponent ) :
        position( position ), ambientComponent( ambientComponent ), diffuseComponent( diffuseComponent ), specularComponent( specularComponent ) {
          Light::counter++;
          id = counter.load();

          eventManager.SHADER_CHANGE.listen( this, std::bind( &Light::send, this ) );
        }

      Light::~Light() {
        Light::counter--;
      }

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
        std::string preamble( "lights[" );
        preamble += std::to_string( id ) + "].";

        glUniform3f(
          Tools::OpenGL::getUniformLocation( preamble + "position" ),
          position[ 0 ], position[ 1 ], position[ 2 ]
        );

        glUniform3f(
          Tools::OpenGL::getUniformLocation( preamble + "ambient" ),
          ambientComponent[ 0 ], ambientComponent[ 1 ], ambientComponent[ 2 ]
        );

        glUniform3f(
          Tools::OpenGL::getUniformLocation( preamble + "diffuse" ),
          diffuseComponent[ 0 ], diffuseComponent[ 1 ], diffuseComponent[ 2 ]
        );

        glUniform3f(
          Tools::OpenGL::getUniformLocation( preamble + "specular" ),
          specularComponent[ 0 ], specularComponent[ 1 ], specularComponent[ 2 ]
        );
      }

    }
  }
}
