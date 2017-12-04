#ifndef SG_MESH_DEFINITION
#define SG_MESH_DEFINITION

#include "graphics/scenegraph/mesh/mesh.hpp"
#include <GL/glew.h>
#include <vector>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      namespace Mesh {

        template < typename VertexType > class MeshDefinition : public Mesh {
        public:
          MeshDefinition( std::vector< VertexType >& vertices, std::vector< GLuint >& indices ) : Mesh::Mesh() {
            glBindVertexArray( VAO );

              glBindBuffer( GL_ARRAY_BUFFER, VBO );
                glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof( VertexType ), &vertices[ 0 ], GL_STATIC_DRAW );
              glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, EBO );
                glBufferData( GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof( GLuint ), &indices[ 0 ], GL_STATIC_DRAW );

              VertexType::setupShaderAttributes();

              glBindBuffer( GL_ARRAY_BUFFER, 0 );

            glBindVertexArray( 0 );
          }

        };

      }
    }
  }
}

#endif
