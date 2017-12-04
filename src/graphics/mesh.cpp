#include "graphics/mesh.hpp"
#include "graphics/armature/armature.hpp"
#include "graphics/transform.hpp"
#include "tools/utility.hpp"
#include "tools/opengl.hpp"
#include "log.hpp"
#include <memory>
#include <vector>
#include <GL/glew.h>
#include <glm/ext.hpp>

namespace BlueBear {
  namespace Graphics {

    Mesh::Mesh(
      std::vector< Vertex >& vertices,
      std::vector< Index >& indices,
      std::vector< std::string > boneIndices,
      std::shared_ptr< Armature > bind
    ) : boneIndices( boneIndices ), bind( bind ), size( indices.size() ) {
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
        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), ( GLvoid* ) 0 );

        glEnableVertexAttribArray( 1 );
        glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), ( GLvoid* ) offsetof( Vertex, normal ) );

        glEnableVertexAttribArray( 2 );
        glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, sizeof( Vertex ), ( GLvoid* ) offsetof( Vertex, textureCoordinates ) );

        glEnableVertexAttribArray( 3 );
        glVertexAttribIPointer( 3, 4, GL_INT, sizeof( Vertex ), ( GLvoid* ) offsetof( Vertex, boneIDs ) );

        glEnableVertexAttribArray( 4 );
        glVertexAttribPointer( 4, 4, GL_FLOAT, GL_FALSE, sizeof( Vertex ), ( GLvoid* ) offsetof( Vertex, boneWeights ) );

        glBindBuffer( GL_ARRAY_BUFFER, 0 );

      glBindVertexArray( 0 );
    }

    void Mesh::drawElements( std::shared_ptr< Armature > currentPose ) {
      std::vector< glm::mat4 > boneUniform;
      // Bone uniform 0 is always identity (for boneless meshes)
      boneUniform.push_back( glm::mat4() );

      if( bind && currentPose ) {
        // Mesh has associated bones that must be computed
        for( std::string& bone : boneIndices ) {
          boneUniform.push_back(
            currentPose->getMatrix( bone ) * glm::inverse( bind->getMatrix( bone ) )
          );
        }
      }

      // Write uniforms to shader
      glUniformMatrix4fv( Tools::OpenGL::getUniformLocation( "bones" ), boneUniform.size(), GL_FALSE, glm::value_ptr( boneUniform[ 0 ] ) );

      glBindVertexArray( VAO );
        glDrawElements( GL_TRIANGLES, size, GL_UNSIGNED_INT, 0 );
      glBindVertexArray( 0 );
    }
  }
}
