#include "graphics/camera.hpp"

#include <string>
#include <sstream>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "localemanager.hpp"
#include "log.hpp"

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

    unsigned int Camera::rotateRight() {
      if( ortho ) {
        currentRotation = ( currentRotation + 1 ) % 4;

        doRotate();
      }

      return currentRotation;
    }

    unsigned int Camera::rotateLeft() {
      if( ortho ) {
        currentRotation = ( currentRotation - 1 ) % 4;

        doRotate();
      }

      return currentRotation;
    }

    GLuint Camera::getCurrentRotation() {
      return currentRotation;
    }

    void Camera::setRotationDirect( GLuint rotation ) {
      if( ortho ) {

        if( rotation > 3 ) {
          Log::getInstance().warn( "Camera::setRotationDirect", "Orthographic rotation not in interval [0,3] (value provided was " + std::to_string( rotation ) + "). Coercing to value in this range..." );
          rotation = rotation % 4;
        }

        currentRotation = rotation;

        doRotate();
      }
    }

    std::string Camera::positionToString() {
      // Yet another GCC bug (surprise, surprise): the identifier "CAMERA" is not usable here because it's confusing it for either
      // "Camera" or "camera", two things that are not any of the other items. GJ GCC!
      static std::string CAMERA_STRING( LocaleManager::getInstance().getString( "CAMERA" ) );

      return CAMERA_STRING + ": " + std::to_string( camera.x ) + ", " + std::to_string( camera.y ) + ", " + std::to_string( camera.z );
    }

    std::string Camera::directionToString() {
      static std::string DIRECTION( LocaleManager::getInstance().getString( "DIRECTION" ) );

      return DIRECTION + ": " + std::to_string( direction.x ) + ", " + std::to_string( direction.y ) + ", " + std::to_string( direction.z );;
    }

  }
}
