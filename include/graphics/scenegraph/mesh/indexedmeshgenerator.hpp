#ifndef INDEXED_MESH_GENERATOR
#define INDEXED_MESH_GENERATOR

#include "graphics/scenegraph/mesh/meshdefinition.hpp"
#include "graphics/scenegraph/mesh/triangle.hpp"
#include <memory>
#include <vector>

#include "log.hpp"
#include "tools/utility.hpp"

namespace BlueBear::Graphics::SceneGraph::Mesh {

  template< typename VertexType >
  class IndexedMeshGenerator {
  protected:
    std::vector< VertexType > vertices;
    std::vector< Triangle > triangles;

    unsigned int insertVertex( VertexType vertex ) {
      for( unsigned int i = 0; i != vertices.size(); i++ ) {
        const VertexType& indexedVertex = vertices[ i ];

        if( vertex == indexedVertex ) {
          return i;
        }
      }

      vertices.push_back( vertex );
      return vertices.size() - 1;
    };

  public:
    void addTriangle( VertexType v1, VertexType v2, VertexType v3 ) {
      triangles.emplace_back( std::move( Triangle{
        insertVertex( v1 ),
        insertVertex( v2 ),
        insertVertex( v3 )
      } ) );
    };

    void generateNormals() {
      std::vector< glm::vec3 > triangleNormals;

      // Generate the normals for each triangle
      for( const auto& triangle : triangles ) {
        triangleNormals.emplace_back(
          glm::cross(
            vertices[ triangle[ 1 ] ].position - vertices[ triangle[ 0 ] ].position,
            vertices[ triangle[ 2 ] ].position - vertices[ triangle[ 0 ] ].position
          )
        );
      }

      // For each vertex, determine what triangles it belongs to and average all their normals
      for( unsigned int vertexIndex = 0; vertexIndex != vertices.size(); vertexIndex++ ) {
        std::vector< unsigned int > triangleIndices;

        // Find indices for each triangle this vertex belongs to
        for( unsigned int triangleIndex = 0; triangleIndex != triangles.size(); triangleIndex++ ) {
          const auto& triangle = triangles.at( triangleIndex );
          for( unsigned int triangleVertexIndex : triangle ) {
            if ( triangleVertexIndex == vertexIndex ) {
              triangleIndices.emplace_back( triangleIndex );
              break;
            }
          }
        }

        // Take the normal average of every triangle this vertex belongs to
        glm::vec3 normalTotal;
        for( unsigned int triangleIndex : triangleIndices ) {
          normalTotal += triangleNormals[ triangleIndex ];
        }

        float numTriangles = triangleIndices.size();
        vertices.at( vertexIndex ).normal = glm::normalize( normalTotal / numTriangles );
      }
    };

    std::shared_ptr< MeshDefinition< VertexType > > generateMesh() {
      // Unroll triangles
      std::vector< unsigned int > unrolledIndices;
      for( const auto& triangle : triangles ) {
        unrolledIndices.push_back( triangle[ 0 ] );
        unrolledIndices.push_back( triangle[ 1 ] );
        unrolledIndices.push_back( triangle[ 2 ] );
      }

      return std::make_shared< MeshDefinition< VertexType > >( vertices, unrolledIndices );
    };
  };

}

#endif
