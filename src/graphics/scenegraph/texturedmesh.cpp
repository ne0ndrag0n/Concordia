#include "graphics/scenegraph/texturedmesh.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {

      TexturedMesh::TexturedMesh( std::vector< TexturedMesh::Vertex >& vertices, std::vector< Mesh::Index >& indices ) {
        glGenVertexArrays( 1, &VAO );
        glGenBuffers( 1, &VBO );
        glGenBuffers( 1, &EBO );

        glBindVertexArray( VAO );
          glBindBuffer( GL_ARRAY_BUFFER, VBO );
            glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof( TexturedMesh::Vertex ), &vertices[ 0 ], GL_STATIC_DRAW );
          glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, EBO );
            glBufferData( GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof( Mesh::Index ), &indices[ 0 ], GL_STATIC_DRAW );

          glEnableVertexAttribArray( 0 );
          glEnableVertexAttribArray( 1 );
          glEnableVertexAttribArray( 2 );
          glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( TexturedMesh::Vertex ), ( GLvoid* ) 0 );
          glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof( TexturedMesh::Vertex ), ( GLvoid* ) offsetof( TexturedMesh::Vertex, normal ) );
          glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, sizeof( TexturedMesh::Vertex ), ( GLvoid* ) offsetof( TexturedMesh::Vertex, textureCoordinates ) );

          glBindBuffer( GL_ARRAY_BUFFER, 0 );

        glBindVertexArray( 0 );
      }

    }
  }
}
