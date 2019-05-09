#ifndef SG_TRANSFORM
#define SG_TRANSFORM

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <sol.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {

      class Transform {
        glm::vec3 position = glm::vec3( 0.0f, 0.0f, 0.0f );
        glm::quat rotation = glm::quat( 1.0f, 0.0f, 0.0f, 0.0f );
        glm::vec3 scale = glm::vec3( 1.0f, 1.0f, 1.0f );
        glm::mat4 result;
        bool dirty = true;

        glm::mat4 getMatrixImmediate() const;
        void recalculate();
        void decompose();

      public:
        static void submitLuaContributions( sol::state& lua );

        Transform();
        Transform( const Transform& transform );
        Transform( const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale );
        Transform( const glm::mat4& existingTransform );
        Transform& operator=( const Transform& rhs );
        Transform operator*( const Transform& rhs ) const;
        Transform& operator*=( const Transform& rhs );

        glm::mat4 getMatrix();

        glm::vec3 getPosition() const;
        void setPosition( const glm::vec3& position );

        glm::vec3 getScale() const;
        void setScale( const glm::vec3& scale );

        glm::quat getRotation() const;
        void setRotation( const glm::quat& rotation );

        static Transform interpolate( const Transform& t1, const Transform& t2, float alpha );
        static glm::mat4 componentsToMatrix( const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale );

        void printToLog();

      };

    }
  }
}

#endif
