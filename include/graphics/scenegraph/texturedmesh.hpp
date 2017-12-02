#ifndef SG_TMESH
#define SG_TMESH

#include "graphics/scenegraph/mesh.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {

      class TexturedMesh : public Mesh {

      public:
        struct Vertex {
          glm::vec3 position;
          glm::vec3 normal;
          glm::vec2 textureCoordinates;
        };

        TexturedMesh( std::vector< TexturedMesh::Vertex >& vertices, std::vector< Mesh::Index >& indices );
      };

    }
  }
}

#endif
