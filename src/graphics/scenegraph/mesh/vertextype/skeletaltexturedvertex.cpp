#include "graphics/scenegraph/mesh/vertextype/skeletaltexturedvertex.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      namespace Mesh {
        namespace VertexType {

          void SkeletalTexturedVertex::setupLayout() {
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
          }

        }
      }
    }
  }
}
