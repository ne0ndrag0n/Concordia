#ifndef SG_MESH_DEFINITION
#define SG_MESH_DEFINITION

#include "graphics/scenegraph/mesh/mesh.hpp"
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

          // Deferred loading of meshes
          std::unique_ptr< const std::vector< VertexType > > deferredVertices;
          std::unique_ptr< const std::vector< GLuint > > deferredIndices;

          // Disable copy
          MeshDefinition( const MeshDefinition& );
          MeshDefinition& operator=( const MeshDefinition& );

          void loadIndexed( const std::vector< VertexType >& vertices, const std::vector< GLuint >& indices ) {
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

          void loadVertices( const std::vector< VertexType >& vertices ) {
            glGenVertexArrays( 1, &VAO );
            glGenBuffers( 1, &VBO );

            glBindVertexArray( VAO );
              glBindBuffer( GL_ARRAY_BUFFER, VBO );

                glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof( VertexType ), &vertices[ 0 ], GL_STATIC_DRAW );
                VertexType::setupShaderAttributes();

              glBindBuffer( GL_ARRAY_BUFFER, 0 );
            glBindVertexArray( 0 );
          }

          void drawIndexed() {
            glDrawElements( GL_TRIANGLES, size, GL_UNSIGNED_INT, 0 );
          }

          void drawVertices() {
            glDrawArrays( GL_TRIANGLES, 0, size );
          }

        public:
          MeshDefinition( const std::vector< VertexType >& vertices, const std::vector< GLuint >& indices, bool defer = false ) :
            size( indices.size() ), indexed( true ), drawMethod( std::bind( &MeshDefinition::drawIndexed, this ) ) {
            getDefaultShader = VertexType::getDefaultShader;

            if( defer ) {
              deferredVertices = std::make_unique< const std::vector< VertexType > >( vertices );
              deferredIndices = std::make_unique< const std::vector< GLuint > >( indices );
            } else {
              loadIndexed( vertices, indices );
            }
          }

          MeshDefinition( const std::vector< VertexType >& vertices, bool defer = false ) :
            size( vertices.size() ), indexed( false ), drawMethod( std::bind( &MeshDefinition::drawVertices, this ) ) {
            getDefaultShader = VertexType::getDefaultShader;

            if( defer ) {
              deferredVertices = std::make_unique< const std::vector< VertexType > >( vertices );
            } else {
              loadVertices( vertices );
            }
          }

          ~MeshDefinition() {
            if( !deferredVertices && !deferredIndices ) {
              glDeleteVertexArrays( 1, &VAO );
              glDeleteBuffers( 1, &VBO );
              if( indexed ) {
                glDeleteBuffers( 1, &EBO );
              }
            }
          }

          void sendDeferred() override {
            if( deferredVertices ) {
              if( deferredIndices ) {
                loadIndexed( *deferredVertices, *deferredIndices );
                deferredVertices = nullptr;
                deferredIndices = nullptr;
              } else {
                loadVertices( *deferredVertices );
                deferredVertices = nullptr;
              }
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
