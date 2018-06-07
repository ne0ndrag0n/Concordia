#include "graphics/scenegraph/light.hpp"
#include "tools/opengl.hpp"
#include "eventmanager.hpp"
#include <GL/glew.h>
#include "log.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {

      std::atomic< unsigned int > Light::counter{ 0 };

      Light::Light( glm::vec3 position, glm::vec3 ambientComponent, glm::vec3 diffuseComponent, glm::vec3 specularComponent ) :
        position( position ), ambientComponent( ambientComponent ), diffuseComponent( diffuseComponent ), specularComponent( specularComponent ) {
          id = counter.load();
          Light::counter++;

          eventManager.SHADER_CHANGE.listen( this, std::bind( &Light::send, this ) );
        }

      Light::~Light() {
        Light::counter--;

        eventManager.SHADER_CHANGE.stopListening( this );
      }

      void Light::sendLightCount() {
        auto location = Tools::OpenGL::getUniformLocation( "numLights" );

        if( location != -1 ) {
          glUniform1ui( location, counter.load() );
        }
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

        Tools::OpenGL::setUniform( preamble + "position", position );
        Tools::OpenGL::setUniform( preamble + "ambient", ambientComponent );
        Tools::OpenGL::setUniform( preamble + "diffuse", diffuseComponent );
        Tools::OpenGL::setUniform( preamble + "specular", specularComponent );
      }

    }
  }
}
