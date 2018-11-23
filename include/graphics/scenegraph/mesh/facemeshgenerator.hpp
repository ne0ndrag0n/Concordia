#ifndef FACE_MESH_GENERATOR
#define FACE_MESH_GENERATOR

#include "exceptions/genexc.hpp"
#include "graphics/scenegraph/mesh/indexedmeshgenerator.hpp"
#include <array>
#include <tuple>

namespace BlueBear::Graphics::SceneGraph::Mesh {

  template< typename VertexType >
  class FaceMeshGenerator : public IndexedMeshGenerator< VertexType > {
    std::unordered_map< std::string, std::vector< Triangle* > > faceCollection;

  public:
    EXCEPTION_TYPE( FaceNotFoundException, "Face not found!" );

    using Change = std::tuple< unsigned int, unsigned int, VertexType >;

    void addFace( const std::string& faceId, const std::vector< std::array< VertexType, 3 > >& collection ) {
      // add triangles
      for( const auto& triangle : collection ) {
        faceCollection[ faceId ].push_back( &this->triangles.emplace_back( std::move( Triangle{
          insertVertex( triangle[ 0 ] ),
          insertVertex( triangle[ 1 ] ),
          insertVertex( triangle[ 2 ] )
        } ) ) );
      }
    }

    VertexType getVertex( const std::string& faceId, unsigned int triangle, unsigned int vertex ) {
      auto it = faceCollection.find( faceId );
      if( it == faceCollection.end() ) {
        throw FaceNotFoundException();
      }

      auto& [ name, list ] = *it;
      Triangle& t = *( list[ triangle ] );
      return t[ vertex ];
    }

    void redefineVertices( const std::string& faceId, const std::vector< Change >& changeList ) {
      auto it = faceCollection.find( faceId );
      if( it == faceCollection.end() ) {
        throw FaceNotFoundException();
      }

      auto& [ name, list ] = *it;
      for( const auto& [ triangleIndex, vertexIndex, vertex ] : changeList ) {
        Triangle& t = *( list[ triangleIndex ] );
        t[ vertexIndex ] = insertVertex( vertex );
      }
    }
  };

}

#endif
