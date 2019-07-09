#include "graphics/scenegraph/transform.hpp"
#include "tools/opengl.hpp"
#include "log.hpp"
#include <glm/ext.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {

      Transform::Transform() {}

      Transform::Transform( const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale ) :
        position( position ), rotation( rotation ), scale( scale ) {}

      Transform::Transform( const glm::mat4& existingTransform ) : result( existingTransform ), dirty( false ) { decompose(); }

      Transform::Transform( const Transform& transform ) : position( transform.position ), rotation( transform.rotation ), scale( transform.scale ), result( transform.result ), dirty( transform.dirty ) {}

      void Transform::submitLuaContributions( sol::state& lua ) {
        sol::table types = lua[ "bluebear" ][ "util" ][ "types" ];

        types.new_usertype< Transform >( "Transform",
          "new", sol::no_constructor,
          "get_position", &Transform::getPosition,
          "get_scale", &Transform::getScale,
          "get_rotation", []( Transform& self ) -> std::tuple< float, glm::vec3 > {
            glm::quat quat = self.getRotation();
            return { glm::angle( quat ), glm::axis( quat ) };
          },
          "set_position", &Transform::setPosition,
          "set_scale", &Transform::setRotation,
          "set_rotation", []( Transform& self, float angle, const glm::vec3& axis ) {
            self.setRotation( glm::angleAxis( glm::radians( angle ), axis ) );
          }
        );
      }

      Transform& Transform::operator=( const Transform& rhs ) {
        position = rhs.position;
        scale = rhs.scale;
        rotation = rhs.rotation;
        result = rhs.result;
        dirty = rhs.dirty;

        return *this;
      }

      Transform Transform::operator*( const Transform& rhs ) const {
        const glm::mat4& thisMatrix = dirty ? getMatrixImmediate() : result;
        const glm::mat4& thatMatrix = rhs.dirty ? rhs.getMatrixImmediate() : rhs.result;

        return Transform( thisMatrix * thatMatrix );
      }

      Transform& Transform::operator*=( const Transform& rhs ) {
        recalculate();

        result *= rhs.getMatrixImmediate();
        decompose();

        return *this;
      }

      glm::mat4 Transform::getMatrixImmediate() const {
        glm::mat4 matrix;

        matrix = glm::mat4( 1.0f );
        matrix *= glm::translate( position );
        matrix *= glm::toMat4( rotation );
        matrix *= glm::scale( scale );

        return matrix;
      }

      void Transform::recalculate() {
        if( dirty ) {
          result = getMatrixImmediate();
          dirty = false;
        }
      }

      void Transform::decompose() {
        // these just get thrown out for now
        glm::vec3 skew;
        glm::vec4 perspective;

        glm::decompose( result, scale, rotation, position, skew, perspective );
        rotation = glm::conjugate( rotation );
      }

      glm::mat4 Transform::getMatrix() {
        recalculate();

        return result;
      }

      glm::mat4 Transform::getMatrix() const {
        if( dirty ) {
          return getMatrixImmediate();
        }

        return result;
      }

      glm::vec3 Transform::getPosition() const {
        return position;
      }

      void Transform::setPosition( const glm::vec3& position ) {
        this->position = position;
        dirty = true;
      }

      glm::vec3 Transform::getScale() const {
        return scale;
      }

      void Transform::setScale( const glm::vec3& scale ) {
        this->scale = scale;
        dirty = true;
      }

      glm::quat Transform::getRotation() const {
        return rotation;
      }

      void Transform::setRotation( const glm::quat& rotation ) {
        this->rotation = rotation;
        dirty = true;
      }

      Transform Transform::interpolate( const Transform& t1, const Transform& t2, float alpha ) {
        return Transform(
          glm::mix( t1.getPosition(), t2.getPosition(), alpha ),
          glm::mix( t1.getRotation(), t2.getRotation(), alpha ),
          glm::mix( t1.getScale(), t2.getScale(), alpha )
        );
      }

      glm::mat4 Transform::componentsToMatrix( const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale ) {
        glm::mat4 quickMatrix;

        quickMatrix = glm::translate( quickMatrix, position );
        quickMatrix = quickMatrix * glm::toMat4( rotation );
        quickMatrix = glm::scale( quickMatrix, scale );

        return quickMatrix;
      }

      void Transform::printToLog() {
        Log::getInstance().debug( "Transform::printToLog", "T: " + glm::to_string( position ) + " S: " + glm::to_string( scale ) + " R: " + glm::to_string( rotation ) + " (angle: " + std::to_string( glm::degrees( glm::angle( rotation ) ) ) + " axis: " + glm::to_string( glm::axis( rotation ) ) + ")" );
      }


    }
  }
}
