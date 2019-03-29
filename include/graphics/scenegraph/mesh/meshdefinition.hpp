#ifndef SG_MESH_DEFINITION
#define SG_MESH_DEFINITION

#include "graphics/scenegraph/mesh/mesh.hpp"
#include <GL/glew.h>
#include <vector>
#include <memory>
#include <map>
#include <functional>

//#include "log.hpp"

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
          bool loaded = false;
          std::function< void() > drawMethod;

          // Deferred loading of meshes
          const std::vector< VertexType > vertices;
          const std::vector< GLuint > indices;

          // Disable copy
          MeshDefinition( const MeshDefinition& );
          MeshDefinition& operator=( const MeshDefinition& );

          void loadIndexed() {
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

            loaded = true;
          }

          void loadVertices() {
            glGenVertexArrays( 1, &VAO );
            glGenBuffers( 1, &VBO );

            glBindVertexArray( VAO );
              glBindBuffer( GL_ARRAY_BUFFER, VBO );

                glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof( VertexType ), &vertices[ 0 ], GL_STATIC_DRAW );
                VertexType::setupShaderAttributes();

              glBindBuffer( GL_ARRAY_BUFFER, 0 );
            glBindVertexArray( 0 );

            loaded = true;
          }

          void drawIndexed() {
            glDrawElements( GL_TRIANGLES, size, GL_UNSIGNED_INT, 0 );
          }

          void drawVertices() {
            glDrawArrays( GL_TRIANGLES, 0, size );
          }

        public:
          MeshDefinition( const std::vector< VertexType >& vertices, const std::vector< GLuint >& indices, bool defer = false ) :
            size( indices.size() ), indexed( true ), drawMethod( std::bind( &MeshDefinition::drawIndexed, this ) ),
            vertices( vertices ), indices( indices ) {
            getDefaultShader = VertexType::getDefaultShader;

            if( !defer ) {
              loadIndexed();
            }

            storeGenericTriangles( vertices, indices );
          }

          MeshDefinition( const std::vector< VertexType >& vertices, bool defer = false ) :
            size( vertices.size() ), indexed( false ), drawMethod( std::bind( &MeshDefinition::drawVertices, this ) ),
            vertices( vertices ) {
            getDefaultShader = VertexType::getDefaultShader;

            if( !defer ) {
              loadVertices();
            }

            storeGenericTriangles( vertices, indices );
          }

          ~MeshDefinition() {
            if( loaded ) {
              glDeleteVertexArrays( 1, &VAO );
              glDeleteBuffers( 1, &VBO );
              if( indexed ) {
                glDeleteBuffers( 1, &EBO );
              }
            }
          }

          void sendDeferred() override {
            if( vertices.size() ) {
              if( indices.size() ) {
                loadIndexed();
              } else {
                loadVertices();
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
