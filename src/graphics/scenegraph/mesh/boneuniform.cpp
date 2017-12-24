#include "graphics/scenegraph/mesh/boneuniform.hpp"
#include "graphics/scenegraph/model.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      namespace Mesh {

        BoneUniform::BoneUniform( const std::vector< std::string >& boneIDs ) : boneIDs( boneIDs ) {}

        void BoneUniform::send() {

        }

      }
    }
  }
}
