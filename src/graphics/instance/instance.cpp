#include "graphics/instance/instance.hpp"
#include "graphics/model.hpp"
#include "graphics/drawable.hpp"
#include <GL/glew.h>
#include <memory>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace BlueBear {
  namespace Graphics {

    Instance::Instance( const Model& model, GLuint shaderProgram ) : shaderProgram( shaderProgram ) {
      prepareInstanceRecursive( model );
    }

    void Instance::prepareInstanceRecursive( const Model& model ) {
      // Copy the drawable
      if( model.drawable ) {
        drawable = std::make_unique< Drawable >( *model.drawable );
      }

      for( auto& pair : model.children ) {
        auto& child = *( pair.second );

        // Hand down the same transform as the parent to this model
        children.emplace( pair.first, std::make_shared< Instance >( child, shaderProgram ) );
      }
    }

    /**
     * FIXME: This only goes one level of recursion deep, you derp
     */
    std::shared_ptr< Instance > Instance::findChildByName( std::string name ) {
      return children[ name ];
    }

    void Instance::drawEntity() {
      // Pass an identity matrix to mix-in
      glm::mat4 identity;
      drawEntity( identity );
    }

    void Instance::drawEntity( glm::mat4& parent ) {
      glm::mat4 nextParent = transform.sendToShader( shaderProgram, parent, dirty );

      if( drawable ) {
        drawable->render( shaderProgram );
      }

      for( auto& pair : children ) {
        // Apply the same transform of the parent
        pair.second->drawEntity( nextParent );
      }
    }

    /**
     * Bit of a hackish way to draw an entity with a "nudge" (temporary position)
     */
    void Instance::nudgeDrawEntity( const glm::vec3& nudge ) {
      Transform originalTransform = transform;

      transform.position += nudge;

      markDirty();
      drawEntity();

      transform = originalTransform;
    }

    void Instance::markDirty() {
      dirty = true;

      for( auto& pair : children ) {
        pair.second->markDirty();
      }
    }

    glm::vec3 Instance::getPosition() {
      return transform.position;
    }

    void Instance::setPosition( const glm::vec3& position ) {
      transform.position = position;
      markDirty();
    }

    glm::vec3 Instance::getScale() {
      return transform.scale;
    }

    void Instance::setScale( const glm::vec3& scale ) {
      transform.scale = scale;
      markDirty();
    }

    glm::vec3 Instance::getRotationAxes() {
      return transform.rotationAxes;
    }

    void Instance::setRotationAxes( const glm::vec3& rotationAxes ) {
      transform.rotationAxes = rotationAxes;
      markDirty();
    }

    GLfloat Instance::getRotationAngle() {
      return transform.rotationAngle;
    }

    void Instance::setRotationAngle( GLfloat rotationAngle ) {
      transform.rotationAngle = rotationAngle;
      markDirty();
    }

  }
}
