#include "graphics/mesh.hpp"
#include <vector>
#include <GL/glew.h>


namespace BlueBear {
  namespace Graphics {

    Mesh::Mesh( std::vector< Vertex >& vertices, std::vector< Index >& indices ) : size( indices.size() ) {
      setupMesh( vertices, indices );
    }

    Mesh::~Mesh() {
      glDeleteVertexArrays( 1, &VAO );
      glDeleteBuffers( 1, &VBO );
      glDeleteBuffers( 1, &EBO );
    }

    void Mesh::setupMesh( std::vector< Vertex >& vertices, std::vector< Index >& indices ) {
      glGenVertexArrays( 1, &VAO );
      glGenBuffers( 1, &VBO );
      glGenBuffers( 1, &EBO );

      glBindVertexArray( VAO );
        glBindBuffer( GL_ARRAY_BUFFER, VBO );
          glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof( Vertex ), &vertices[ 0 ], GL_STATIC_DRAW );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, EBO );
          glBufferData( GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof( Index ), &indices[ 0 ], GL_STATIC_DRAW );

        glEnableVertexAttribArray( 0 );
        glEnableVertexAttribArray( 1 );
        glEnableVertexAttribArray( 2 );
        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), ( GLvoid* ) 0 );
        glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), ( GLvoid* ) offsetof( Vertex, normal ) );
        glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, sizeof( Vertex ), ( GLvoid* ) offsetof( Vertex, textureCoordinates ) );

        glBindBuffer( GL_ARRAY_BUFFER, 0 );

      glBindVertexArray( 0 );
    }

    void Mesh::drawElements() {
      glBindVertexArray( VAO );
        glDrawElements( GL_TRIANGLES, size, GL_UNSIGNED_INT, 0 );
      glBindVertexArray( 0 );
    }

  }
}
