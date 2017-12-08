#include "graphics/scenegraph/mesh/vertextype/skeletaltexturedvertex.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      namespace Mesh {
        namespace VertexType {

          void SkeletalTexturedVertex::setupShaderAttributes(
            GLuint VBO,
            GLuint EBO,
            const std::vector< SkeletalTexturedVertex >& vertices,
            const std::vector< GLuint >& indices
          ) {
            glBindBuffer( GL_ARRAY_BUFFER, VBO );
              glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof( SkeletalTexturedVertex ), &vertices[ 0 ], GL_STATIC_DRAW );
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, EBO );
              glBufferData( GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof( GLuint ), &indices[ 0 ], GL_STATIC_DRAW );

            glEnableVertexAttribArray( 0 );
            glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( SkeletalTexturedVertex ), ( GLvoid* ) 0 );

            glEnableVertexAttribArray( 1 );
            glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof( SkeletalTexturedVertex ), ( GLvoid* ) offsetof( SkeletalTexturedVertex, normal ) );

            glEnableVertexAttribArray( 2 );
            glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, sizeof( SkeletalTexturedVertex ), ( GLvoid* ) offsetof( SkeletalTexturedVertex, textureCoordinates ) );

            glEnableVertexAttribArray( 3 );
            glVertexAttribIPointer( 3, 4, GL_INT, sizeof( SkeletalTexturedVertex ), ( GLvoid* ) offsetof( SkeletalTexturedVertex, boneIDs ) );

            glEnableVertexAttribArray( 4 );
            glVertexAttribPointer( 4, 4, GL_FLOAT, GL_FALSE, sizeof( SkeletalTexturedVertex ), ( GLvoid* ) offsetof( SkeletalTexturedVertex, boneWeights ) );

            glBindBuffer( GL_ARRAY_BUFFER, 0 );
          }

        }
      }
    }
  }
}
