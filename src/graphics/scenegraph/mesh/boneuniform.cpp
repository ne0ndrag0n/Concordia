#include "graphics/scenegraph/mesh/boneuniform.hpp"
#include "graphics/scenegraph/model.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      namespace Mesh {

        BoneUniform::BoneUniform( const std::vector< std::string >& boneIDs, std::shared_ptr< Model > armature ) : boneIDs( boneIDs ), armature( armature ) {}

        void BoneUniform::send() {

        }

      }
    }
  }
}
