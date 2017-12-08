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
            StorageMethod::allocate( &VAO, &VBO, &EBO );

            glBindVertexArray( VAO );
              VertexType::setupShaderAttributes( VBO, EBO, vertices, indices );
            glBindVertexArray( 0 );
          }

          ~Mesh() {
            StorageMethod::deallocate( &VAO, &VBO, &EBO );
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
