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
      Armature( aiNode* armatureNode, bool invert = false );
      struct BoneNotFoundException : public std::exception {
        const char* what() const throw() {
          return "Bone ID not found!";
        }
      };

      glm::mat4 getMatrix( const std::string& id );

    private:
      void loadLevel( aiNode* node, Skeleton& currentLevel, glm::mat4 parentTransform );

      glm::mat4* getMatrixProxy( const std::string& id, Skeleton& level );
    };

  }
}

#endif
