#include "graphics/instance/instance.hpp"
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
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace BlueBear {
  namespace Graphics {

    Instance::Instance( const Model& model ) {
      animationList = std::make_shared< AnimationList >();

      prepareInstanceRecursive( model );
    }

    Instance::Instance( const Model& model, std::shared_ptr< AnimationList > animationList )
      : animationList( animationList ) {
      prepareInstanceRecursive( model );
    }

    void Instance::prepareInstanceRecursive( const Model& model ) {
      // Copy the drawable
      if( model.drawable ) {
        drawable = std::make_shared< Drawable >( *model.drawable );
      }

      transform = Transform( model.transform );

      for( auto& pair : model.children ) {
        auto& child = *( pair.second );

        // Hand down the same transform as the parent to this model
        std::shared_ptr< Instance > instance = std::make_shared< Instance >( child, animationList );
        instance->transform.setParent( &transform );

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
     *
     */
    void Instance::setAnimation( const std::string& animKey ) {
      // TODO
    }

    /**
     * Public-facing overload
     */
    void Instance::drawEntity() {
      drawEntity( false, *this );
    }

    void Instance::drawEntity( bool dirty, Instance& rootInstance ) {
      dirty = dirty || transform.dirty;

      // Update if dirty
      if( dirty ) {
        transform.update();
      }

      if( drawable ) {
        transform.sendToShader();
        drawable->render();
      }

      for( auto& pair : children ) {
        // If "dirty" was true here, it'll get passed down to subsequent instances. But if "dirty" was false, and this call ends up being "dirty",
        // it should only propagate to its own children since dirty is passed by value here.
        pair.second->drawEntity( dirty, rootInstance );
      }
    }

    glm::vec3 Instance::getPosition() {
      return transform.getPosition();
    }

    void Instance::setPosition( const glm::vec3& position ) {
      transform.setPosition( position );
    }

    void Instance::setPositionBy( const glm::vec3& position ) {
      transform.setPosition( transform.getPosition() + position );
    }

    glm::vec3 Instance::getScale() {
      return transform.getScale();
    }

    void Instance::setScale( const glm::vec3& scale ) {
      transform.setScale( scale );
    }

    GLfloat Instance::getRotationAngle() {
      return transform.getRotationAngle();
    }

    glm::vec3 Instance::getRotationAxes() {
      return transform.getRotationAxes();
    }

    void Instance::setRotationAngle( GLfloat rotationAngle, const glm::vec3& rotationAxes ) {
      // Generate new quat
      transform.setRotationAngle( rotationAngle, rotationAxes );
    }

  }
}
