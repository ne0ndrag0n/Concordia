#ifndef SG_BONE_UNIFORM
#define SG_BONE_UNIFORM

#include "graphics/scenegraph/uniform.hpp"
#include <glm/glm.hpp>
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
          std::vector< glm::mat4 > boneUniform;

        public:
          BoneUniform( const std::vector< std::string >& boneIDs );
          void configure( std::shared_ptr< Animation::Animator > animator, std::shared_ptr< Model > parent );
          void send() override;
        };

      }
    }
  }
}

#endif
