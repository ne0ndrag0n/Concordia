#ifndef GFXTRANSFORM
#define GFXTRANSFORM

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

namespace BlueBear {
  namespace Graphics {
    class Transform {

      public:
        glm::vec3 position = glm::vec3( 0.0f, 0.0f, 0.0f );
        glm::vec3 scale = glm::vec3( 1.0f, 1.0f, 1.0f );
        glm::vec3 rotationAxes = glm::vec3( 0.0f, 0.0f, 1.0f );
        GLfloat rotationAngle = 0.0f;
        glm::mat4 matrix;

        Transform();

        glm::mat4 sendToShader( GLuint shaderProgram, glm::mat4& mixin, bool& recalculate );
    };
  }
}

#endif
