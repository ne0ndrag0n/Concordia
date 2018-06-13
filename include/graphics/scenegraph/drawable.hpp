#ifndef SG_DRAWABLE
#define SG_DRAWABLE

#include <memory>

namespace BlueBear::Graphics { class Shader; }
namespace BlueBear::Graphics::SceneGraph {
  class Material;
  namespace Mesh { class Mesh; }

  struct Drawable {
    std::shared_ptr< Mesh::Mesh > mesh;
    std::shared_ptr< Shader > shader;
    std::shared_ptr< Material > material;

    operator bool() const {
      return mesh && material && shader;
    }
  };

}

#endif
