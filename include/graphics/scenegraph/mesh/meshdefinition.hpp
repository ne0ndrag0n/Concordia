#ifndef SG_MESH_DEFINITION
#define SG_MESH_DEFINITION

#include "graphics/scenegraph/mesh/mesh.hpp"
#include "log.hpp"
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
          bool loaded = false;
          std::function< void() > drawMethod;
          std::function< glm::vec3( const VertexType& ) > genericTransformMethod;

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
            genericTransformMethod( std::bind( &MeshDefinition::defaultGenericTransform, this, std::placeholders::_1 ) ),
            vertices( vertices ), indices( indices ) {
            getDefaultShader = VertexType::getDefaultShader;

            if( !defer ) {
              loadIndexed();
            }
          }

          MeshDefinition( const std::vector< VertexType >& vertices, bool defer = false ) :
            size( vertices.size() ), indexed( false ), drawMethod( std::bind( &MeshDefinition::drawVertices, this ) ),
            genericTransformMethod( std::bind( &MeshDefinition::defaultGenericTransform, this, std::placeholders::_1 ) ),
            vertices( vertices ) {
            getDefaultShader = VertexType::getDefaultShader;

            if( !defer ) {
              loadVertices();
            }
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

          std::vector< Geometry::Triangle > getTriangles() override {
            std::vector< Geometry::Triangle > genericTriangles;

            if( indices.size() ) {
              bool check = indices.size() % 3 == 0;
              if( check ) {
                for( size_t i = 0; i < indices.size(); i += 3 ) {
                  genericTriangles.emplace_back( Geometry::Triangle{
                    genericTransformMethod( vertices[ indices[ i ] ] ),
                    genericTransformMethod( vertices[ indices[ i + 1 ] ] ),
                    genericTransformMethod( vertices[ indices[ i + 2 ] ] )
                  } );
                }
              } else {
                Log::getInstance().warn( "MeshDefinition::storeGenericTriangles", "Mesh indices are not a multiple of 3; skipping generic triangle generation." );
              }
            } else {
              bool check = vertices.size() % 3 == 0;
              if( check ) {
                for( size_t i = 0; i < vertices.size(); i += 3 ) {
                  genericTriangles.emplace_back( Geometry::Triangle{
                    genericTransformMethod( vertices[ i ] ),
                    genericTransformMethod( vertices[ i + 1 ] ),
                    genericTransformMethod( vertices[ i + 2 ] )
                  } );
                }
              } else {
                Log::getInstance().warn( "MeshDefinition::storeGenericTriangles", "Mesh vertices are not a multiple of 3; skipping generic triangle generation." );
              }
            }

            return genericTriangles;
          }

          glm::vec3 defaultGenericTransform( const VertexType& vertex ) {
            return vertex.position;
          }

          void setGenericTransformMethod( const std::function< glm::vec3( const VertexType& ) >& method ) {
            if( method ) {
              genericTransformMethod = method;
            } else {
              genericTransformMethod = std::bind( &MeshDefinition::defaultGenericTransform, this, std::placeholders::_1 );
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

          void drawElements( const Shader& shader ) override {
            for( auto& pair : meshUniforms ) {
              pair.second->send( shader );
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
