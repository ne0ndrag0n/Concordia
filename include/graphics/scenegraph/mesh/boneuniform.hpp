#ifndef SG_BONE_UNIFORM
#define SG_BONE_UNIFORM

#include "graphics/scenegraph/mesh/meshuniform.hpp"
#include <vector>
#include <string>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      namespace Mesh {

        class BoneUniform : public MeshUniform {
          std::vector< std::string > boneIDs;

        public:
          BoneUniform( const std::vector< std::string >& boneIDs );
          void send() override;
        };

      }
    }
  }
}

#endif
