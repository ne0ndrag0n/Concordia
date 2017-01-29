#ifndef ARMATURE
#define ARMATURE

#include "graphics/armature/skeleton.hpp"
#include <assimp/scene.h>
#include <exception>

namespace BlueBear {
  namespace Graphics {

    class Armature {
    public:
      Skeleton skeletons;

      void invertSkeletons( Skeleton& currentLevel );
      Armature( aiNode* armatureNode );
      struct BoneNotFoundException : public std::exception {
        const char* what() const throw() {
          return "Bone ID not found!";
        }
      };

      glm::mat4 getMatrix( const std::string& id );
      void replaceMatrix( const std::string& id, glm::mat4 replacement );

    private:
      struct BoneDiscovery {
        glm::mat4 hierarchy;
        glm::mat4* result;
      };
      void loadLevel( aiNode* node, Skeleton& currentLevel );

      BoneDiscovery getMatrixProxy( const std::string& id, Skeleton& level, glm::mat4 parent );
    };

  }
}

#endif
