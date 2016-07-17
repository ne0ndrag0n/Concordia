#include "graphics/transform.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

namespace BlueBear {
  namespace Graphics {

    Transform::Transform() {}

    glm::mat4 Transform::sendToShader( GLuint shaderProgram, glm::mat4& mixin, bool& recalculate ) {

      if( recalculate ) {
        matrix = glm::mat4();

        // Mixin the parent matrix
        matrix = matrix * mixin;

        // There's always going to be a translation
        matrix = glm::translate( matrix, position );

        // Then rotate
        matrix = glm::rotate( matrix, rotationAngle, rotationAxes );

        // Then scale
        matrix = glm::scale( matrix, scale );

        recalculate = false;
      }

      // Set the uniform for the shader
      glUniformMatrix4fv( glGetUniformLocation( shaderProgram, "model" ), 1, GL_FALSE, glm::value_ptr( matrix ) );

      return matrix;
    }

  }
}
