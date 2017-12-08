#ifndef SG_MESH_DEFINITION
#define SG_MESH_DEFINITION

#include "graphics/scenegraph/mesh/meshuniform.hpp"
#include <GL/glew.h>
#include <vector>
#include <memory>
#include <map>
#include <functional>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      namespace Mesh {

        template < typename VertexType > class Mesh {
          GLuint VAO;
          GLuint VBO;
          GLuint EBO;
          unsigned int size;
          bool indexed;

          std::function< void() > drawMethod;

          // Disable copy
          Mesh( const Mesh& );
          Mesh& operator=( const Mesh& );

          void drawIndexed() {
            glDrawElements( GL_TRIANGLES, size, GL_UNSIGNED_INT, 0 );
          }

          void drawVertices() {
            // TODO: forgot how to draw triangles without EBO
          }

        public:
          std::map< std::string, std::unique_ptr< MeshUniform > > meshUniforms;

          Mesh( const std::vector< VertexType >& vertices, const std::vector< GLuint >& indices ) :
            size( indices.size() ), indexed( true ), drawMethod( &Mesh::drawIndexed ) {
            glGenVertexArrays( 1, &VAO );
            glGenBuffers( 1, &VBO );
            glGenBuffers( 1, &EBO );

            glBindVertexArray( VAO );
              glBindBuffer( GL_ARRAY_BUFFER, VBO );

                glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof( VertexType ), &vertices[ 0 ], GL_STATIC_DRAW );
                VertexType::setupLayout();

                glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, EBO );
                glBufferData( GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof( GLuint ), &indices[ 0 ], GL_STATIC_DRAW );

              glBindBuffer( GL_ARRAY_BUFFER, 0 );
            glBindVertexArray( 0 );
          }

          Mesh( const std::vector< VertexType >& vertices ) :
            size( vertices.size() ), indexed( false ), drawMethod( &Mesh::drawVertices ) {
            glGenVertexArrays( 1, &VAO );
            glGenBuffers( 1, &VBO );

            glBindVertexArray( VAO );
              glBindBuffer( GL_ARRAY_BUFFER, VBO );

                glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof( VertexType ), &vertices[ 0 ], GL_STATIC_DRAW );
                VertexType::setupLayout();

              glBindBuffer( GL_ARRAY_BUFFER, 0 );
            glBindVertexArray( 0 );
          }

          ~Mesh() {
            glDeleteVertexArrays( 1, &VAO );
            glDeleteBuffers( 1, &VBO );
            if( indexed ) {
              glDeleteBuffers( 1, &EBO );
            }
          }

          void drawElements() {
            for( auto& pair : meshUniforms ) {
              pair.second->send();
            }

            glBindVertexArray( VAO );
              drawMethod();
            glBindVertexArray( 0 );
          }

        };

      }
    }
  }
}

#endif
