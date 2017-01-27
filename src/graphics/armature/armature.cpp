#include "graphics/armature/armature.hpp"
#include "graphics/model.hpp"
#include <glm/glm.hpp>

namespace BlueBear {
  namespace Graphics {

    Armature::Armature( aiNode* armatureNode, bool invert ) {
      loadLevel( armatureNode, skeletons, glm::mat4() );

      if( invert ) {
        invertSkeletons();
      }
    }

    void Armature::invertSkeletons() {
      // TODO
    }

    void Armature::loadLevel( aiNode* node, Skeleton& currentLevel, glm::mat4 cumulativeTransform ) {
      for( int i = 0; i < node->mNumChildren; i++ ) {
        aiNode* boneNode = node->mChildren[ i ];
        glm::mat4 boneTransform = cumulativeTransform * Model::aiToGLMmat4( boneNode->mTransformation );

        Bone& newBone = skeletons[ boneNode->mName.C_Str() ] = Bone{ boneTransform, Skeleton() };

        loadLevel( boneNode, newBone.children, boneTransform );
      }
    }

  }
}
