#ifndef ARMATURE
#define ARMATURE

#include "graphics/armature/skeleton.hpp"
#include <assimp/scene.h>

namespace BlueBear {
  namespace Graphics {

    class Armature {
    public:
      Skeleton skeletons;

      void invertSkeletons( Skeleton& currentLevel );
      Armature( aiNode* armatureNode, bool invert = false );

    private:
      void loadLevel( aiNode* node, Skeleton& currentLevel, glm::mat4 parentTransform );
    };

  }
}

#endif
