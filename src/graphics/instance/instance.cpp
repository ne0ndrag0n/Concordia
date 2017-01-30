#include "graphics/instance/instance.hpp"
#include "graphics/armature/armature.hpp"
#include "graphics/model.hpp"
#include "graphics/drawable.hpp"
#include "graphics/animplayer.hpp"
#include "tools/utility.hpp"
#include "tools/opengl.hpp"
#include "log.hpp"
#include <GL/glew.h>
#include <memory>
#include <string>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace BlueBear {
  namespace Graphics {

    Instance::Instance( const Model& model ) : transform( std::make_shared< Transform >() ) {
      setRootLevelItems( model );

      prepareInstanceRecursive( model );
    }

    Instance::Instance( const Model& model, bool noRoot ) : transform( std::make_shared< Transform >() ) {
      prepareInstanceRecursive( model );
    }

    void Instance::setRootLevelItems( const Model& root ) {
      bindPose = currentPose = root.bind;

      animations = root.animations;
    }

    void Instance::prepareInstanceRecursive( const Model& model ) {
      // Copy the drawable
      if( model.drawable ) {
        drawable = std::make_shared< Drawable >( *model.drawable );
      }

      for( auto& pair : model.children ) {
        auto& child = *( pair.second );

        // Hand down the same transform as the parent to this model
        std::shared_ptr< Instance > instance = std::make_shared< Instance >( child, true );
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


    /**
     * setAnimation will act the same whether or not it's called on a child node or the root node. It will activate all animations as if called from the top of the scene graph.
     */
    void Instance::setAnimation( const std::string& animKey ) {
      if( !animations ) {
        Log::getInstance().warn( "Instance::setAnimation", "This instance has no animations." );
        return;
      }

      auto it = animations->find( animKey );
      if( it == animations->end() ) {
        Log::getInstance().warn( "Instance::setAnimation", "Instance does not have animation with ID " + animKey );
        return;
      }

      currentAnimation = std::make_shared< AnimPlayer >( it->second );
    }

    /**
     * Update the animation pose if an AnimPlayer is attached.
     */
    void Instance::updateAnimationPose() {
      if( currentAnimation ) {
        // Use currentAnimation to get new Armature based off the bind pose
        currentPose = currentAnimation->generateNextFrame( bindPose );

        // When we get back the bind pose, we're done playing this animation
        if( currentPose == bindPose ) {
          currentAnimation = nullptr;
        }
      }
    }

    /**
     * Public-facing overload
     */
    void Instance::drawEntity() {
      updateAnimationPose();

      drawEntity( false, *this );
    }

    void Instance::drawEntity( bool dirty, Instance& rootInstance ) {
      dirty = dirty || transform->dirty;

      // Update if dirty
      if( dirty ) {
        transform->update();
      }

      if( drawable ) {
        transform->sendToShader();
        drawable->render( rootInstance.currentPose );
      }

      for( auto& pair : children ) {
        // If "dirty" was true here, it'll get passed down to subsequent instances. But if "dirty" was false, and this call ends up being "dirty",
        // it should only propagate to its own children since dirty is passed by value here.
        pair.second->drawEntity( dirty, rootInstance );
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
