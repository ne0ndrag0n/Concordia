#include "graphics/camera.hpp"
#include "tools/opengl.hpp"
#include <string>
#include <sstream>
#include <functional>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "graphics/shader.hpp"
#include "localemanager.hpp"
#include "eventmanager.hpp"
#include "log.hpp"

namespace BlueBear {
  namespace Graphics {

    Camera::Camera( int screenWidth, int screenHeight ) : shaderCamera( 0 ) {
      widthHalf = ( (float)screenWidth / 2 );
      heightHalf = ( (float)screenHeight / 2 );
    }

    void Camera::setPosition( const glm::vec3& position ) {
      camera = position;
      dirty = true;
    }

    const glm::vec3& Camera::getPosition() const {
      return camera;
    }

    void Camera::move( GLfloat x, GLfloat y, GLfloat z ) {
      camera += glm::vec3( x, y, z );
      dirty = true;
    }

    void Camera::doRotate() {
      // FIXME: Consider making this more flexible. A lot of stuff up to this point depends on the legacy 0-1-2-3 rotation
      // which previously assumed a gluLookAt method of computing the view matrix (see the "rotations" array). This switch statement is garbage.
      switch( currentRotation ) {
        case 0:
          orthoRotationAngle = 45.0f;
          break;
        case 1:
          orthoRotationAngle = 315.0f;
          break;
        case 2:
          orthoRotationAngle = 225.0f;
          break;
        case 3:
        default:
          orthoRotationAngle = 135.0f;
          break;
      }

      dirty = true;

      CAMERA_ROTATED.trigger();
    }

    float Camera::zoomIn() {
      if( zoom != 2.0f ) {
        zoom += zoomIncrement;
        dirty = true;
      }

      return zoom;
    }

    float Camera::zoomOut() {
      if( zoom != 0.25f ) {
        zoom -= zoomIncrement;
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
        // getOrthoView for now
        view = getOrthoView();
        projection = getOrthoMatrix();

        sendToShader();

        dirty = false;
      }
    }

    const Camera::CameraUniformBundle& Camera::getUniforms( const Shader* shader ) {
      auto it = shaders.find( shader );
      if( it != shaders.end() ) {
        return it->second;
      }

      CameraUniformBundle& bundle = shaders[ shader ];
      bundle.cameraPosUniform = shader->getUniform( "cameraPos" );
      bundle.viewUniform = shader->getUniform( "view" );
      bundle.projectionUniform = shader->getUniform( "projection" );
      return bundle;
    }

    void Camera::sendToShader() {
      // new stuff
      shaderCamera.update( [ & ]( Std140Camera& camera ) {
        camera.cameraPos = glm::vec4( this->camera, 0.0f );
        camera.view = view;
        camera.projection = projection;
      } );
    }

    glm::mat4 Camera::getOrthoView() {
      glm::mat4 view;

      view = glm::translate( view, camera );

      view = glm::rotate( view, glm::radians( -60.0f ), glm::vec3( 1.0f, 0.0f, 0.0f ) );
      view = glm::rotate( view, glm::radians( orthoRotationAngle ), glm::vec3( 0.0f, 0.0f, 1.0f ) );

      view = glm::scale( view, glm::vec3{ zoom * 100.0f, zoom * 100.0f, zoom * 100.0f } );

      return view;
    }

    glm::mat4 Camera::getOrthoMatrix() {
      glm::mat4 ortho;
      glm::vec2 scaledCoordinates = getScaledCoordinates();

      ortho = glm::ortho( -scaledCoordinates.x, scaledCoordinates.x, -scaledCoordinates.y, scaledCoordinates.y, -10000.0f, 10000.0f );

      return ortho;
    }

    glm::vec2 Camera::getScaledCoordinates() const {
      return { widthHalf, heightHalf };
    }

    unsigned int Camera::rotateRight() {
      glm::mat4 transform;
      transform = glm::rotate( transform, glm::radians( 90.0f ), glm::vec3{ 0.0f, 0.0f, 1.0f } );
      camera = transform * glm::vec4( camera, 0.0f );

      currentRotation = ( currentRotation - 1 ) % 4;
      doRotate();
      return currentRotation;
    }

    unsigned int Camera::rotateLeft() {
      glm::mat4 transform;
      transform = glm::rotate( transform, glm::radians( -90.0f ), glm::vec3{ 0.0f, 0.0f, 1.0f } );
      camera = transform * glm::vec4( camera, 0.0f );

      currentRotation = ( currentRotation + 1 ) % 4;
      doRotate();
      return currentRotation;
    }

    GLuint Camera::getCurrentRotation() {
      return currentRotation;
    }

    void Camera::setRotationDirect( GLuint rotation ) {
      if( rotation > 3 ) {
        Log::getInstance().warn( "Camera::setRotationDirect", "Orthographic rotation not in interval [0,3] (value provided was " + std::to_string( rotation ) + "). Coercing to value in this range..." );
        rotation = rotation % 4;
      }

      currentRotation = rotation;

      doRotate();
    }

    Geometry::Ray Camera::getPickingRay( glm::ivec2 mouseLocation, const glm::uvec2& screenDimensions ) {
      Geometry::Ray ray;

      glm::mat4 viewMatrix = getOrthoView();
      glm::mat4 projectionMatrix = getOrthoMatrix();

      mouseLocation.y = screenDimensions.y - mouseLocation.y;

      glm::vec3 nearPlane = glm::unProject(
        glm::vec3( mouseLocation.x, mouseLocation.y, 0.0f ),
        viewMatrix,
        projectionMatrix,
        glm::vec4( 0.0f, 0.0f, screenDimensions.x, screenDimensions.y )
      );

      glm::vec3 farPlane = glm::unProject(
        glm::vec3( mouseLocation.x, mouseLocation.y, 1.0f ),
        viewMatrix,
        projectionMatrix,
        glm::vec4( 0.0f, 0.0f, screenDimensions.x, screenDimensions.y )
      );

      ray.origin = nearPlane;
      ray.direction = glm::normalize( farPlane - nearPlane );

      return ray;
    }

    float Camera::getRotationAngle() const {
      return orthoRotationAngle;
    }

  }
}
