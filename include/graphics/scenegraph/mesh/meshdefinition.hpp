#ifndef SG_MESH_DEFINITION
#define SG_MESH_DEFINITION

#include "graphics/scenegraph/mesh/mesh.hpp"
#include "graphics/scenegraph/uniform.hpp"
#include <GL/glew.h>
#include <vector>
#include <memory>
#include <map>
#include <functional>

namespace BlueBear {
  namespace Graphics {
    class Shader;

    namespace SceneGraph {
      namespace Mesh {

        template < typename VertexType > class MeshDefinition : public Mesh {
          GLuint VAO;
          GLuint VBO;
          GLuint EBO;
          unsigned int size;
          bool indexed;
          std::function< void() > drawMethod;

          // Disable copy
          MeshDefinition( const MeshDefinition& );
          MeshDefinition& operator=( const MeshDefinition& );

          void drawIndexed() {
            glDrawElements( GL_TRIANGLES, size, GL_UNSIGNED_INT, 0 );
          }

          void drawVertices() {
            glDrawArrays( GL_TRIANGLES, 0, size );
          }

        public:
          std::map< std::string, std::unique_ptr< Uniform > > meshUniforms;

          MeshDefinition( const std::vector< VertexType >& vertices, const std::vector< GLuint >& indices ) :
            size( indices.size() ), indexed( true ), drawMethod( std::bind( &MeshDefinition::drawIndexed, this ) ) {
            getDefaultShader = VertexType::getDefaultShader;

            glGenVertexArrays( 1, &VAO );
            glGenBuffers( 1, &VBO );
            glGenBuffers( 1, &EBO );

            glBindVertexArray( VAO );
              glBindBuffer( GL_ARRAY_BUFFER, VBO );

                glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof( VertexType ), &vertices[ 0 ], GL_STATIC_DRAW );
                VertexType::setupShaderAttributes();

                glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, EBO );
                glBufferData( GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof( GLuint ), &indices[ 0 ], GL_STATIC_DRAW );

              glBindBuffer( GL_ARRAY_BUFFER, 0 );
            glBindVertexArray( 0 );
          }

          MeshDefinition( const std::vector< VertexType >& vertices ) :
            size( vertices.size() ), indexed( false ), drawMethod( std::bind( &MeshDefinition::drawVertices, this ) ) {
            getDefaultShader = VertexType::getDefaultShader;

            glGenVertexArrays( 1, &VAO );
            glGenBuffers( 1, &VBO );

            glBindVertexArray( VAO );
              glBindBuffer( GL_ARRAY_BUFFER, VBO );

                glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof( VertexType ), &vertices[ 0 ], GL_STATIC_DRAW );
                VertexType::setupShaderAttributes();

              glBindBuffer( GL_ARRAY_BUFFER, 0 );
            glBindVertexArray( 0 );
          }

          ~MeshDefinition() {
            glDeleteVertexArrays( 1, &VAO );
            glDeleteBuffers( 1, &VBO );
            if( indexed ) {
              glDeleteBuffers( 1, &EBO );
            }
          }

          void drawElements() override {
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
