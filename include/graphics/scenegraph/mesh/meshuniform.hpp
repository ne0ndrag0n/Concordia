#ifndef SG_MESH_UNIFORM
#define SG_MESH_UNIFORM

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      namespace Mesh {

        class MeshUniform {
        public:
          virtual void send() = 0;
        };

      }
    }
  }
}

#endif
