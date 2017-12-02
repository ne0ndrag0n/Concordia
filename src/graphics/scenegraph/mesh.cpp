#include "graphics/scenegraph/mesh.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <GL/glew.h>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {

      Mesh::Mesh( std::vector< Mesh::Vertex >& vertices, std::vector< Mesh::Index >& indices ) : size( indices.size() ) {
        glGenVertexArrays( 1, &VAO );
        glGenBuffers( 1, &VBO );
        glGenBuffers( 1, &EBO );

        glBindVertexArray( VAO );
          glBindBuffer( GL_ARRAY_BUFFER, VBO );
            glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof( Mesh::Vertex ), &vertices[ 0 ], GL_STATIC_DRAW );
          glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, EBO );
            glBufferData( GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof( Mesh::Index ), &indices[ 0 ], GL_STATIC_DRAW );

          glEnableVertexAttribArray( 0 );
          glEnableVertexAttribArray( 1 );
          glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( Mesh::Vertex ), ( GLvoid* ) 0 );
          glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof( Mesh::Vertex ), ( GLvoid* ) offsetof( Mesh::Vertex, normal ) );

          glBindBuffer( GL_ARRAY_BUFFER, 0 );

        glBindVertexArray( 0 );
      }

      Mesh::~Mesh() {
        glDeleteVertexArrays( 1, &VAO );
        glDeleteBuffers( 1, &VBO );
        glDeleteBuffers( 1, &EBO );
      }

      void Mesh::draw() {
        glBindVertexArray( VAO );
          glDrawElements( GL_TRIANGLES, size, GL_UNSIGNED_INT, 0 );
        glBindVertexArray( 0 );
      }

    }
  }
}
