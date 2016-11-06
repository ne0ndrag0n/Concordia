#ifndef MODELINSTANCE
#define MODELINSTANCE

#include "graphics/transform.hpp"
#include "graphics/drawable.hpp"
#include <string>
#include <memory>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

namespace BlueBear {
  namespace Graphics {
    class Model;

    /**
     * A GFXInstance is a specific instance of a graphic model placed on a lot. It contains
     * a position vector and other associated states (depending on the type of model).
     */
    class Instance {

      private:
        GLuint shaderProgram;
        Transform transform;
        bool dirty = true;

        void prepareInstanceRecursive( const Model& model );

      public:
        std::unique_ptr< Drawable > drawable;
        std::map< std::string, std::shared_ptr< Instance > > children;

        Instance( const Model& model, GLuint shaderProgram );

        std::shared_ptr< Instance > findChildByName( std::string name );

        void drawEntity();

        void drawEntity( glm::mat4& parent );

        void nudgeDrawEntity( const glm::vec3& nudge );

        void markDirty();

        glm::vec3 getPosition();

        void setPosition( const glm::vec3& position );

        glm::vec3 getScale();

        void setScale( const glm::vec3& scale );

        glm::vec3 getRotationAxes();

        void setRotationAxes( const glm::vec3& rotationAxes );

        GLfloat getRotationAngle();

        void setRotationAngle( GLfloat rotationAngle );
    };
  }
}
#endif
