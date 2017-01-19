#include "graphics/transform.hpp"
#include "tools/opengl.hpp"
#include "log.hpp"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

namespace BlueBear {
  namespace Graphics {

    Transform::Transform() {}

    Transform::Transform( const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale ) :
      position( position ), rotation( rotation ), scale( scale ) {}

    Transform::Transform( const glm::mat4& existingTransform ) : matrix( existingTransform ) {

      // these just get thrown out for now
      glm::vec3 skew;
      glm::vec4 perspective;

      glm::decompose( matrix, scale, rotation, position, skew, perspective );
      rotation = glm::conjugate( rotation );
    }

    void Transform::update() {
      // When we update, CLEAR THE MATRIX!
      matrix = glm::mat4();
      localMatrix = glm::mat4();

      glm::mat4 mixin;

      if( parent ) {
        mixin = parent->matrix;
      }

      // Mixin the parent matrix
      matrix = matrix * mixin;

      // There's always going to be a translation
      localMatrix = glm::translate( localMatrix, position );

      // Then rotate
      localMatrix = localMatrix * glm::toMat4( rotation );

      // Then scale
      localMatrix = glm::scale( localMatrix, scale );

      matrix = matrix * localMatrix;

      // This is now updated.
      dirty = false;
    }

    glm::mat4 Transform::getUpdatedMatrix() {
      update();

      return matrix;
    }

    /**
     * Get matrix a slightly more efficient way (don't generate the matrix again if not dirty)
     */
    glm::mat4 Transform::getMatrix() {
      if( dirty ) {
        update();
      }

      return matrix;
    }

    void Transform::sendToShader() {
      // Set the uniform for the shader
      glUniformMatrix4fv( Tools::OpenGL::getUniformLocation( "model" ), 1, GL_FALSE, glm::value_ptr( matrix ) );
    }

    void Transform::setParent( std::shared_ptr< Transform > parent ) {
      this->parent = parent;
    }

    glm::vec3 Transform::getPosition() {
      return position;
    }

    void Transform::setPosition( const glm::vec3& position ) {
      this->position = position;
      dirty = true;
    }

    glm::vec3 Transform::getScale() {
      return scale;
    }

    void Transform::setScale( const glm::vec3& scale ) {
      this->scale = scale;
      dirty = true;
    }

    GLfloat Transform::getRotationAngle() {
      return glm::angle( rotation );
    }

    glm::vec3 Transform::getRotationAxes() {
      return glm::axis( rotation );
    }

    void Transform::setRotationAngle( GLfloat rotationAngle, const glm::vec3& rotationAxes ) {
      // Generate new quat
      rotation = glm::angleAxis( rotationAngle, rotationAxes );
      dirty = true;
    }

    void Transform::setRotation( const glm::quat& rotation ) {
      this->rotation = rotation;
    }

    glm::quat Transform::getRotation() {
      return rotation;
    }

    Transform Transform::interpolate( Transform& t1, Transform& t2, float alpha ) {
      return Transform(
        glm::mix( t1.getPosition(), t2.getPosition(), alpha ),
        glm::mix( t1.getRotation(), t2.getRotation(), alpha ),
        glm::mix( t1.getScale(), t2.getScale(), alpha )
      );
    }

    void Transform::printToLog() {
      Log::getInstance().debug( "Transform::printToLog", std::string( "\n" ) +
        "Transform Info: " + "\n" +
        "Position: " + glm::to_string( position ) + "\n" +
        "Scale: " + glm::to_string( scale ) + "\n" +
        "Rotation: " + glm::to_string( rotation )
      );
    }

  }
}
