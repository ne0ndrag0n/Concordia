#include "tools/assimptools.hpp"
#include <assimp/postprocess.h>

namespace BlueBear {
  namespace Tools {

    glm::mat4 AssimpTools::aiToGLMmat4( aiMatrix4x4& matrix ) {
      glm::mat4 result;

      result[ 0 ] = glm::vec4( matrix[ 0 ][ 0 ], matrix[ 1 ][ 0 ], matrix[ 2 ][ 0 ], matrix[ 3 ][ 0 ] );
      result[ 1 ] = glm::vec4( matrix[ 0 ][ 1 ], matrix[ 1 ][ 1 ], matrix[ 2 ][ 1 ], matrix[ 3 ][ 1 ] );
      result[ 2 ] = glm::vec4( matrix[ 0 ][ 2 ], matrix[ 1 ][ 2 ], matrix[ 2 ][ 2 ], matrix[ 3 ][ 2 ] );
      result[ 3 ] = glm::vec4( matrix[ 0 ][ 3 ], matrix[ 1 ][ 3 ], matrix[ 2 ][ 3 ], matrix[ 3 ][ 3 ] );

      return result;
    }

    glm::vec4 AssimpTools::aiToGLMvec4( aiVector3D& vector ) {
      glm::vec4 result;

      result.x = vector.x;
      result.y = vector.y;
      result.z = vector.z;
      result.w = 1.0f;

      return result;
    }

    glm::vec3 AssimpTools::aiToGLMvec3( aiVector3D& vector ) {
      glm::vec3 result;

      result.x = vector.x;
      result.y = vector.y;
      result.z = vector.z;

      return result;
    }

    glm::dquat AssimpTools::aiToGLMquat( aiQuaternion& quaternion ) {
      glm::dquat result;

      result.x = quaternion.x;
      result.y = quaternion.y;
      result.z = quaternion.z;
      result.w = quaternion.w;

      return result;
    }

  }
}
