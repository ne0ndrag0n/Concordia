#ifndef SG_MESH
#define SG_MESH

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      namespace Mesh {

        class Mesh {
        public:
          virtual void drawElements() = 0;
        };

      }
    }
  }
}

#endif
