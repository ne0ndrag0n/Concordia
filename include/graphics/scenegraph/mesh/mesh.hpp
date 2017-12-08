#ifndef SG_MESH_DEFINITION
#define SG_MESH_DEFINITION

#include "graphics/scenegraph/mesh/meshuniform.hpp"
#include <GL/glew.h>
#include <vector>
#include <memory>
#include <map>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      namespace Mesh {

        template < typename VertexType, typename StorageMethod > class Mesh {
          using Index = GLuint;

          GLuint VAO;
          GLuint VBO;
          GLuint EBO;
          unsigned int size;

          // Disable copy
          Mesh( const Mesh& );
          Mesh& operator=( const Mesh& );

        public:
          std::map< std::string, std::unique_ptr< MeshUniform > > meshUniforms;

          Mesh( const std::vector< VertexType >& vertices, const std::vector< GLuint >& indices ) {
            glGenVertexArrays( 1, &VAO );
            glGenBuffers( 1, &VBO );
            StorageMethod::allocateIndices( &EBO );

            glBindVertexArray( VAO );
              glBindBuffer( GL_ARRAY_BUFFER, VBO );

                glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof( VertexType ), &vertices[ 0 ], GL_STATIC_DRAW );
                VertexType::setupLayout();

                StorageMethod::uploadIndices( EBO, indices );

              glBindBuffer( GL_ARRAY_BUFFER, 0 );
            glBindVertexArray( 0 );
          }

          ~Mesh() {
            glDeleteVertexArrays( 1, &VAO );
            glDeleteBuffers( 1, &VBO );
            StorageMethod::deallocateIndices( &EBO );
          }

          void drawElements() {
            for( auto& pair : meshUniforms ) {
              pair.second->send();
            }

            glBindVertexArray( VAO );
              StorageMethod::draw( size );
            glBindVertexArray( 0 );
          }

        };

      }
    }
  }
}

#endif
