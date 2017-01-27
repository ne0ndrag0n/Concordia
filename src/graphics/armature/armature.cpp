#include "graphics/armature/armature.hpp"
#include "graphics/model.hpp"
#include "log.hpp"
#include <glm/glm.hpp>

namespace BlueBear {
  namespace Graphics {

    Armature::Armature( aiNode* armatureNode, bool invert ) {
      loadLevel( armatureNode, skeletons, glm::mat4() );

      if( invert ) {
        invertSkeletons( skeletons );
      }
    }

    void Armature::invertSkeletons( Skeleton& currentLevel ) {
      for( auto& pair : currentLevel ) {
        pair.second.transform = glm::inverse( pair.second.transform );

        invertSkeletons( pair.second.children );
      }
    }

    void Armature::loadLevel( aiNode* node, Skeleton& currentLevel, glm::mat4 parentTransform ) {
      for( int i = 0; i < node->mNumChildren; i++ ) {
        aiNode* boneNode = node->mChildren[ i ];
        glm::mat4 boneTransform = parentTransform * Model::aiToGLMmat4( boneNode->mTransformation );

        Bone& newBone = currentLevel[ boneNode->mName.C_Str() ] = Bone{ boneTransform, Skeleton() };

        loadLevel( boneNode, newBone.children, boneTransform );
      }
    }

  }
}
