#ifndef MESH_TEXTUREMAPPER
#define MESH_TEXTUREMAPPER

#include <vector>

namespace BlueBear::Graphics::SceneGraph::Mesh::TextureMapper {

  template< typename VertexType >
  class TextureMapper {
  protected:
    std::vector< VertexType >& vertices;

  public:
    TextureMapper( std::vector< VertexType >& vertices ) : vertices( vertices ) {}

    virtual void map() = 0;
  };

}

#endif
