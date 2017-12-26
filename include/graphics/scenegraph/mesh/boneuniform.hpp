#ifndef SG_BONE_UNIFORM
#define SG_BONE_UNIFORM

#include "graphics/scenegraph/uniform.hpp"
#include <vector>
#include <string>
#include <memory>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      class Model;

      namespace Animation {
        class Animator;
      }

      namespace Mesh {

        class BoneUniform : public Uniform {
          std::vector< std::string > boneIDs;
          std::weak_ptr< Animation::Animator > animator;

        public:
          BoneUniform( const std::vector< std::string >& boneIDs, std::shared_ptr< Animation::Animator > animator );
          void send() override;
        };

      }
    }
  }
}

#endif
