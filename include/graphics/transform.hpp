#ifndef GFXTRANSFORM
#define GFXTRANSFORM

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <memory>

namespace BlueBear {
  namespace Graphics {
    class Transform {

      std::shared_ptr< Transform > parent;
      glm::vec3 position = glm::vec3( 0.0f, 0.0f, 0.0f );
      glm::vec3 scale = glm::vec3( 1.0f, 1.0f, 1.0f );
      glm::quat rotation = glm::quat( 1.0f, 0.0f, 0.0f, 0.0f );

      public:
        bool dirty = true;
        glm::mat4 matrix;

        Transform();
        Transform( const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale );
        Transform( const glm::mat4& existingTransform );

        void update( const glm::mat4& composure = glm::mat4() );
        glm::mat4 getUpdatedMatrix();
        void sendToShader( GLuint shaderProgram );

        void setParent( std::shared_ptr< Transform > parent );

        // new methods to change position, scale, and rotation
        glm::vec3 getPosition();

        void setPosition( const glm::vec3& position );

        glm::vec3 getScale();

        void setScale( const glm::vec3& scale );

        GLfloat getRotationAngle();

        glm::vec3 getRotationAxes();

        void setRotationAngle( GLfloat rotationAngle, const glm::vec3& rotationAxes = glm::vec3( 0.0f, 0.0f, 1.0f ) );

        void setRotation( const glm::quat& rotation );

        glm::quat getRotation();
    };
  }
}

#endif
