#ifndef SG_BONE_UNIFORM
#define SG_BONE_UNIFORM

#include "graphics/scenegraph/uniform.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <map>
#include <string>
#include <memory>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      class Model;

      namespace Animation {
        class BonePackage;
      }

      namespace Mesh {

        class BoneUniform : public Uniform {
          std::vector< std::string > boneIDs;
          std::vector< glm::mat4 > boneUniform;

        public:
          BoneUniform( const std::vector< std::string >& boneIDs );
          void configure( const std::map< std::string, glm::mat4 >& computedBones );
          const std::vector< glm::mat4 >& getBoneList() const;
          void send() override;
        };

      }
    }
  }
}

#endif
