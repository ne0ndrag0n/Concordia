#ifndef SG_BONE_UNIFORM
#define SG_BONE_UNIFORM

#include "graphics/scenegraph/uniform.hpp"
#include "graphics/shader.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <unordered_map>

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

          std::unordered_map< const void*, Shader::Uniform > uniforms;
          Shader::Uniform getUniform( const Shader* shader );

        public:
          BoneUniform( const std::vector< std::string >& boneIDs );
          void configure( const std::map< std::string, glm::mat4 >& computedBones );
          const std::vector< glm::mat4 >& getBoneList() const;
          void send( const Shader& shader ) override;
        };

      }
    }
  }
}

#endif
