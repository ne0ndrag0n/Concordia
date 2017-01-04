#include "graphics/transform.hpp"
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

    void Transform::update( const glm::mat4& composure ) {
      glm::mat4 mixin;

      if( parent ) {
        mixin = parent->matrix;
      }

      // Mixin any prior composure (animations)
      matrix = matrix * composure;

      // Mixin the parent matrix
      matrix = matrix * mixin;

      // There's always going to be a translation
      matrix = glm::translate( matrix, position );

      // Then rotate
      matrix = matrix * glm::toMat4( rotation );

      // Then scale
      matrix = glm::scale( matrix, scale );

      // This is now updated.
      dirty = false;
    }

    glm::mat4 Transform::getUpdatedMatrix() {
      update();

      return matrix;
    }

    void Transform::sendToShader( GLuint shaderProgram ) {
      // Set the uniform for the shader
      glUniformMatrix4fv( glGetUniformLocation( shaderProgram, "model" ), 1, GL_FALSE, glm::value_ptr( matrix ) );
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

  }
}
