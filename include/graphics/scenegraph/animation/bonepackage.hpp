#ifndef SG_BONEPACKAGE
#define SG_BONEPACKAGE

namespace BlueBear::Graphics::SceneGraph::Animation {
  class Bone;

  struct BonePackage {
    const Bone& bindSkeleton;
    const Bone& currentSkeleton;
  };

}

#endif
