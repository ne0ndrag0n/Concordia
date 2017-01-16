#ifndef MODELINSTANCE
#define MODELINSTANCE

#include "graphics/transform.hpp"
#include "graphics/drawable.hpp"
#include "graphics/bone.hpp"
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
        std::shared_ptr< std::map< std::string, KeyframeBundle > > animations;
        std::shared_ptr< AnimPlayer > animPlayer;

        // Again, lazy
        using BoneList = std::vector< Bone< Instance > >;
        using ModelBoneList = std::vector< Bone< Model > >;
        using AnimationList = std::map< std::string, std::vector< std::shared_ptr< Instance > > >;
        std::shared_ptr< BoneList > boneList;
        std::shared_ptr< AnimationList > animationList;

        void prepareInstanceRecursive( const Model& model, std::shared_ptr< ModelBoneList > modelBones );
        void drawEntity( bool dirty, bool sentBones, Instance& rootInstance );
        void findMatchingSubmodel( std::shared_ptr< ModelBoneList > modelBones, std::shared_ptr< Model > model, std::shared_ptr< Instance > inst );
        void sendBonesToShader( bool& sentBones, Instance& instance );

      public:
        std::shared_ptr< Drawable > drawable;
        std::map< std::string, std::shared_ptr< Instance > > children;

        Instance( const Model& model );
        Instance( const Model& model, std::shared_ptr< BoneList > boneList, std::shared_ptr< ModelBoneList > modelBones, std::shared_ptr< AnimationList > animationList );

        void setAnimation( const std::string& animKey );

        std::shared_ptr< Instance > findChildByName( std::string name );

        void drawEntity();

        glm::vec3 getPosition();

        void setPosition( const glm::vec3& position );

        glm::vec3 getScale();

        void setScale( const glm::vec3& scale );

        GLfloat getRotationAngle();

        glm::vec3 getRotationAxes();

        void setRotationAngle( GLfloat rotationAngle, const glm::vec3& rotationAxes = glm::vec3( 0.0f, 0.0f, 1.0f ) );
    };
  }
}
#endif
