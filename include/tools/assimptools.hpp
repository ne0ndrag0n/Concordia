#ifndef CC_ASSIMP_TOOLS
#define CC_ASSIMP_TOOLS

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <assimp/matrix4x4.h>
#include <assimp/vector3.h>
#include <assimp/quaternion.h>

namespace BlueBear {
  namespace Tools {

    struct AssimpTools {
      static glm::mat4 aiToGLMmat4( aiMatrix4x4& matrix );
      static glm::vec4 aiToGLMvec4( aiVector3D& vector );
      static glm::vec3 aiToGLMvec3( aiVector3D& vector );
      static glm::dquat aiToGLMquat( aiQuaternion& quaternion );
    };

  }
}

#endif
