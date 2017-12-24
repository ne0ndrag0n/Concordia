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

      namespace Mesh {

        class BoneUniform : public Uniform {
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
