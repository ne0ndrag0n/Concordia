#ifndef SG_TRANSFORM
#define SG_TRANSFORM

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {

      class Transform {
        glm::vec3 position = glm::vec3( 0.0f, 0.0f, 0.0f );
        glm::vec3 scale = glm::vec3( 1.0f, 1.0f, 1.0f );
        glm::quat rotation = glm::quat( 1.0f, 0.0f, 0.0f, 0.0f );
        glm::mat4 result;
        bool dirty = true;

        void recalculate();

      public:
        Transform();
        Transform( const Transform& transform );
        Transform( const glm::vec3& position, const glm::vec3& scale, const glm::quat& rotation );
        Transform( const glm::mat4& existingTransform );
        Transform& operator=( const Transform& other );

        glm::mat4 getMatrix();

        glm::vec3 getPosition();
        void setPosition( const glm::vec3& position );

        glm::vec3 getScale();
        void setScale( const glm::vec3& scale );

        glm::quat getRotation();
        void setRotation( const glm::quat& rotation );

        void send( const glm::mat4& parentMixin );

        static Transform interpolate( Transform& t1, Transform& t2, float alpha );
        static glm::mat4 componentsToMatrix( const glm::vec3& position, const glm::vec3& scale, const glm::quat& rotation );

      };

    }
  }
}

#endif
