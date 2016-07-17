#include "graphics/camera.hpp"

#include <string>
#include <sstream>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace BlueBear {
  namespace Graphics {

    Camera::Camera( GLuint program, int screenWidth, int screenHeight ) : program( program ) {
      widthHalf = ( (float)screenWidth / 2 );
      heightHalf = ( (float)screenHeight / 2 );
      perspectiveAspectRatio = (float)screenWidth / (float)screenHeight;
    }

    void Camera::move( GLfloat x, GLfloat y, GLfloat z ) {
      camera += glm::vec3( x, y, z );
      dirty = true;
    }

    void Camera::doRotate() {
      glm::vec2 quadrant = rotations[ currentRotation ];

      camera.x = cameraHeight * quadrant.x;
      camera.y = cameraHeight * quadrant.y;
      direction = originalDirection = glm::vec3( glm::normalize( lookingAt - camera ) );
      dirty = true;
    }

    bool Camera::setOrthographic( bool flag ) {
      ortho = flag;

      if( ortho ) {
        direction = originalDirection;
      }

      dirty = true;
      return ortho;
    }

    float Camera::zoomIn() {
      if( zoom != 1.0f ) {
        zoom -= zoomIncrement;
        dirty = true;
      }

      return zoom;
    }

    float Camera::zoomOut() {
      if( zoom != 3.0f ) {
        zoom += zoomIncrement;
        dirty = true;
      }

      return zoom;
    }

    float Camera::setZoom( float zoomSetting ) {
      dirty = true;
      return zoom = zoomSetting;
    }

    void Camera::position() {
      if( dirty ) {
        GLfloat scaledWidthHalf = ( widthHalf * zoom ) / 100.0f;
        GLfloat scaledHeightHalf = ( heightHalf * zoom ) / 100.0f;

        view = glm::lookAt( camera, camera + direction, up );
        projection = ortho ?
           glm::ortho( -scaledWidthHalf, scaledWidthHalf, -scaledHeightHalf, scaledHeightHalf, -20.0f, 50.0f ) :
           glm::perspective( 45.0f, perspectiveAspectRatio, 0.1f, 50.0f );

        dirty = false;
      }
      // Set uniforms
      glUniformMatrix4fv( glGetUniformLocation( program, "view" ), 1, GL_FALSE, glm::value_ptr( view ) );
      glUniformMatrix4fv( glGetUniformLocation( program, "projection" ), 1, GL_FALSE, glm::value_ptr( projection ) );
    }

    void Camera::walkForward() {
      camera += glm::vec3( direction.x * 0.1f, direction.y * 0.1f, 0.0f );
      dirty = true;
    }

    void Camera::walkBackward() {
      camera -= glm::vec3( direction.x * 0.1f, direction.y * 0.1f, 0.0f );
      dirty = true;
    }

    void Camera::updateFirstPersonView( int deltaX, int deltaY ) {
      if( !( deltaX == 0 && deltaY == 0 ) ) {
        // These are the correct settings for an up normal of Y, not Z
        // Adjust them to work for an up normal of Z...
        GLfloat xOffset = -deltaX * 0.07f;
        GLfloat yOffset = -deltaY * 0.07f;

        // Flip because our world is ass-backwards
        yaw += xOffset;
        pitch += yOffset;

        if( pitch > 89.0f ) {
          pitch = 89.0f;
        }
        if( pitch < -89.0f ) {
          pitch = -89.0f;
        }

        glm::vec3 newDirection(
          cos( glm::radians( yaw ) ) * cos( glm::radians( pitch ) ),
          sin( glm::radians( yaw ) ) * cos( glm::radians( pitch ) ),
          sin( glm::radians( pitch ) )
        );
        direction = glm::normalize( newDirection );
        dirty = true;
      }
    }

    void Camera::rotateRight() {
      if( ortho ) {
        currentRotation = ( currentRotation - 1 ) % 4;

        doRotate();
      }
    }

    void Camera::rotateLeft() {
      if( ortho ) {
        currentRotation = ( currentRotation + 1 ) % 4;

        doRotate();
      }
    }

    std::string Camera::positionToString() {
      std::stringstream s;
      s << "Camera: " << camera.x << ", " << camera.y << ", " << camera.z;
      return s.str();
    }

    std::string Camera::directionToString() {
      std::stringstream s;
      s << "Direction: " << direction.x << ", " << direction.y << ", " << direction.z << std::endl;
      return s.str();
    }

  }
}
