#ifndef CAMERA
#define CAMERA

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

namespace BlueBear {
  namespace Graphics {
    class Shader;

    class Camera {

      private:
        glm::mat4 view;
        glm::mat4 projection;

        glm::vec3 camera = glm::vec3( 0, 0, 10.0f );
        GLuint currentRotation = 0;

        float widthHalf;
        float heightHalf;
        float zoom = 1.0f;
        const float zoomIncrement = 0.25f;
        bool dirty = true;

        GLfloat orthoRotationAngle = 45.0f;

        void doRotate();

      public:
        Camera( int screenWidth, int screenHeight );
        void move( GLfloat x, GLfloat y, GLfloat z );
        float zoomIn();
        float zoomOut();
        float setZoom( float zoomSetting );
        void position();
        void sendToShader();
        glm::mat4 getOrthoView();
        glm::mat4 getOrthoMatrix();
        unsigned int rotateRight();
        unsigned int rotateLeft();
        GLuint getCurrentRotation();
        void setRotationDirect( GLuint rotation );
    };
  }
}

#endif
