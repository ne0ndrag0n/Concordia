#ifndef MODELINSTANCE
#define MODELINSTANCE

#include "graphics/transform.hpp"
#include "graphics/drawable.hpp"
#include "graphics/keyframebundle.hpp"
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

namespace BlueBear {
  namespace Graphics {
    class Armature;
    class Model;
    class KeyframeBundle;
    class AnimPlayer;

    /**
     * A GFXInstance is a specific instance of a graphic model placed on a lot. It contains
     * a position vector and other associated states (depending on the type of model).
     */
    class Instance {

      private:
        std::shared_ptr< Transform > transform;
        std::shared_ptr< AnimPlayer > animPlayer;

        // Root-level items
        std::shared_ptr< Armature > bindPose;
        std::shared_ptr< Armature > currentPose;
        std::shared_ptr< std::map< std::string, Animation > > animations;
        std::shared_ptr< AnimPlayer > currentAnimation;

        void prepareInstanceRecursive( const Model& model );
        void drawEntity( bool dirty, Instance& rootInstance );

        void setRootLevelItems( const Model& root );

      public:
        std::shared_ptr< Drawable > drawable;
        std::map< std::string, std::shared_ptr< Instance > > children;

        Instance( const Model& model );
        Instance( const Model& model, bool noRoot );

        void updateAnimationPose();

        void setAnimation( const std::string& animKey, bool playNow = true );

        std::string getAnimation();

        void setAnimationFrame( double frame );

        std::shared_ptr< Instance > findChildByName( std::string name );

        void drawEntity();

        glm::vec3 getPosition();

        void setPosition( const glm::vec3& position );

        glm::vec3 getScale();

        void setScale( const glm::vec3& scale );

        GLfloat getRotationAngle();

        glm::vec3 getRotationAxes();

        void setRotationAngle( GLfloat rotationAngle, const glm::vec3& rotationAxes = glm::vec3( 0.0f, 0.0f, 1.0f ) );

        std::shared_ptr< std::map< std::string, Animation > > getAnimList();

        std::shared_ptr< AnimPlayer > getAnimPlayer();
    };
  }
}
#endif
