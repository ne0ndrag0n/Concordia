#include "graphics/instance/instance.hpp"
#include "graphics/model.hpp"
#include "graphics/drawable.hpp"
#include "graphics/animplayer.hpp"
#include "log.hpp"
#include <GL/glew.h>
#include <memory>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace BlueBear {
  namespace Graphics {

    Instance::Instance( const Model& model, GLuint shaderProgram ) : transform( std::make_shared< Transform >() ), shaderProgram( shaderProgram ) {
      prepareInstanceRecursive( model );
    }

    void Instance::prepareInstanceRecursive( const Model& model ) {
      // Copy the drawable
      if( model.drawable ) {
        drawable = std::make_shared< Drawable >( *model.drawable );
      }

      // Copy the anim pointer
      animations = model.animations;

      for( auto& pair : model.children ) {
        auto& child = *( pair.second );

        // Hand down the same transform as the parent to this model
        auto instance = std::make_shared< Instance >( child, shaderProgram );
        instance->transform->setParent( transform );

        children[ pair.first ] = instance;
      }
    }

    /**
     * FIXME: This only goes one level of recursion deep, you derp
     */
    std::shared_ptr< Instance > Instance::findChildByName( std::string name ) {
      return children[ name ];
    }

    void Instance::setAnimation( const std::string& animKey ) {
      if( !animations ) {
        Log::getInstance().warn( "Instance::setAnimation", "Tried to play animation " + animKey + " but node doesn't have animations." );
        return;
      }

      auto pair = animations->find( animKey );
      if( pair == animations->end() ) {
        Log::getInstance().warn( "Instance::setAnimation", "Tried to play animation " + animKey + " but it doesn't exist on this node." );
        return;
      }

      animPlayer = std::make_shared< AnimPlayer >( pair->second );
    }

    void Instance::drawEntity( bool dirty ) {

      // If we find one transform at this level that's dirty, every subsequent transform needs to get updated
      dirty = dirty || transform->dirty || ( animPlayer && animPlayer->generateNextFrame() );

      // Update if dirty
      if( dirty ) {
        transform->update( animPlayer ? animPlayer->nextMatrix : glm::mat4() );
      }

      if( drawable ) {
        transform->sendToShader( shaderProgram );
        drawable->render( shaderProgram );
      }

      for( auto& pair : children ) {
        // If "dirty" was true here, it'll get passed down to subsequent instances. But if "dirty" was false, and this call ends up being "dirty",
        // it should only propagate to its own children since dirty is passed by value here.
        pair.second->drawEntity( dirty );
      }
    }

    glm::vec3 Instance::getPosition() {
      return transform->getPosition();
    }

    void Instance::setPosition( const glm::vec3& position ) {
      transform->setPosition( position );
    }

    glm::vec3 Instance::getScale() {
      return transform->getScale();
    }

    void Instance::setScale( const glm::vec3& scale ) {
      transform->setScale( scale );
    }

    GLfloat Instance::getRotationAngle() {
      return transform->getRotationAngle();
    }

    glm::vec3 Instance::getRotationAxes() {
      return transform->getRotationAxes();
    }

    void Instance::setRotationAngle( GLfloat rotationAngle, const glm::vec3& rotationAxes ) {
      // Generate new quat
      transform->setRotationAngle( rotationAngle, rotationAxes );
    }

  }
}
