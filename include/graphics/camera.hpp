#ifndef CAMERA
#define CAMERA

#include "geometry/ray.hpp"
#include "graphics/shader.hpp"
#include "eventmanager.hpp"
#include <unordered_map>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

namespace BlueBear {
  namespace Graphics {
    class Camera {

      private:
        glm::mat4 view;
        glm::mat4 projection;

        glm::vec3 camera = glm::vec3( 0, 0, 0.0f );
        GLuint currentRotation = 0;

        float widthHalf;
        float heightHalf;
        float zoom = 1.0f;
        const float zoomIncrement = 0.25f;
        bool dirty = true;

        GLfloat orthoRotationAngle = 45.0f;

        struct CameraUniformBundle {
          Shader::Uniform cameraPosUniform;
          Shader::Uniform viewUniform;
          Shader::Uniform projectionUniform;
        };

        std::unordered_map< const void*, CameraUniformBundle > shaders;

        const CameraUniformBundle& getUniforms( const Shader* shader );
        void doRotate();
        void sendToShader( const Shader& shader );

      public:
        BasicEvent< void* > CAMERA_ROTATED;

        Camera( int screenWidth, int screenHeight );
        const glm::vec3& getPosition() const;
        void move( GLfloat x, GLfloat y, GLfloat z );
        float zoomIn();
        float zoomOut();
        float setZoom( float zoomSetting );
        void position();
        glm::mat4 getOrthoView();
        glm::mat4 getOrthoMatrix();
        glm::vec2 getScaledCoordinates() const;
        unsigned int rotateRight();
        unsigned int rotateLeft();
        GLuint getCurrentRotation();
        void setRotationDirect( GLuint rotation );

        Geometry::Ray getPickingRay( glm::ivec2 mouseLocation, const glm::uvec2& screenDimensions );
        float getRotationAngle() const;
    };
  }
}

#endif
