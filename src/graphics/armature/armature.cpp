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

    glm::mat4 Armature::getMatrix( const std::string& id ) {
      glm::mat4* intermediate = getMatrixProxy( id, skeletons );

      if( intermediate ) {
        return *intermediate;
      }

      throw BoneNotFoundException();
    }

    glm::mat4* Armature::getMatrixProxy( const std::string& id, Skeleton& level ) {
      // Exhibit A: Why RAII is shit

      auto it = level.find( id );

      if( it != level.end() ) {
        return &it->second.transform;
      }

      for( auto& pair : level ) {
        Bone& bone = pair.second;

        glm::mat4* result = getMatrixProxy( id, bone.children );
        if( result ) {
          return result;
        }
      }

      return nullptr;

    }

  }
}
