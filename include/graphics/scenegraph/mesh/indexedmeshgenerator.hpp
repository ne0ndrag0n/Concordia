#ifndef INDEXED_MESH_GENERATOR
#define INDEXED_MESH_GENERATOR

#include "graphics/scenegraph/mesh/meshdefinition.hpp"
#include <memory>
#include <vector>
#include <array>

namespace BlueBear::Graphics::SceneGraph::Mesh {

  template< typename VertexType >
  class IndexedMeshGenerator {
    std::vector< VertexType > vertices;
    std::vector< std::array< unsigned int, 3 > > triangles;

    unsigned int insertVertex( VertexType vertex ) {
      for( unsigned int i = 0; i != vertices.size(); i++ ) {
        const VertexType& indexedVertex = vertices[ i ];

        if( vertex.position == indexedVertex.position ) {
          return i;
        }
      }

      vertices.push_back( vertex );
      return vertices.size() - 1;
    };

  public:
    void addTriangle( VertexType v1, VertexType v2, VertexType v3 ) {
      triangles.emplace_back( std::move( std::array< unsigned int, 3 >{
        insertVertex( v1 ),
        insertVertex( v2 ),
        insertVertex( v3 )
      } ) );
    };

    void generateNormals() {
      // TODO !!
    };

    std::shared_ptr< MeshDefinition< VertexType > > generateMesh() {
      return nullptr;
    };
  };

}

#endif
