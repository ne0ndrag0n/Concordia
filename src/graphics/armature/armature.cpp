#include "graphics/armature/armature.hpp"
#include "graphics/model.hpp"
#include "log.hpp"
#include <glm/glm.hpp>

namespace BlueBear {
  namespace Graphics {

    Armature::Armature( aiNode* armatureNode ) {
      loadLevel( armatureNode, skeletons );
    }

    void Armature::loadLevel( aiNode* node, Skeleton& currentLevel ) {
      for( int i = 0; i < node->mNumChildren; i++ ) {
        aiNode* boneNode = node->mChildren[ i ];

        Bone& newBone = currentLevel[ boneNode->mName.C_Str() ] = Bone{ Model::aiToGLMmat4( boneNode->mTransformation ), Skeleton() };

        loadLevel( boneNode, newBone.children );
      }
    }

    glm::mat4 Armature::getMatrix( const std::string& id ) {
      BoneDiscovery intermediate = getMatrixProxy( id, skeletons, glm::mat4() );

      if( intermediate.result ) {
        return intermediate.hierarchy * *intermediate.result;
      }

      Log::getInstance().error( "Armature::getMatrix", "Could not locate bone " + id + " in this armature." );
      throw BoneNotFoundException();
    }

    void Armature::replaceMatrix( const std::string& id, glm::mat4 replacement ) {
      BoneDiscovery foundTransform = getMatrixProxy( id, skeletons, glm::mat4() );

      if( foundTransform.result ) {
        *foundTransform.result = replacement;
      } else {
        Log::getInstance().error( "Armature::replaceMatrix", "Could not locate bone " + id + " in this armature." );

        throw BoneNotFoundException();
      }
    }

    Armature::BoneDiscovery Armature::getMatrixProxy( const std::string& id, Skeleton& level, glm::mat4 parent ) {
      // Exhibit A: Why RAII is shit

      auto it = level.find( id );

      if( it != level.end() ) {
        return BoneDiscovery{ parent, &it->second.transform };
      }

      for( auto& pair : level ) {
        Bone& bone = pair.second;

        BoneDiscovery result = getMatrixProxy( id, bone.children, parent * bone.transform );
        if( result.result ) {
          return result;
        }
      }

      // The value of hierarchy for a BoneDiscovery when result is nullptr is unusable and undefined behaviour
      return BoneDiscovery{ parent, nullptr };

    }

  }
}
