#ifndef SG_BONE
#define SG_BONE

#include <glm/glm.hpp>
#include <vector>
#include <string>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      namespace Mesh {

        struct Bone {
          std::string id;
          glm::mat4 matrix;
          std::vector< Bone > children;

          const Bone* getChildById( const std::string& id ) const;
        };

      }
    }
  }
}

#endif
