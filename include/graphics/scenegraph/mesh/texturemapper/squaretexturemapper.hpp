#ifndef MESH_SQUAREMAPPER
#define MESH_SQUAREMAPPER

#include "graphics/scenegraph/mesh/texturemapper/texturemapper.hpp"
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <limits>

namespace BlueBear::Graphics::SceneGraph::Mesh::TextureMapper {

  template< typename VertexType >
  class SquareTextureMapper : public TextureMapper< VertexType > {

    glm::vec2 getMin() {
      glm::vec2 result = { std::numeric_limits< float >::max(), std::numeric_limits< float >::max() };

      for( const auto& vertex : this->vertices ) {
        result.x = std::min( result.x, vertex.position.x );
        result.y = std::min( result.y, vertex.position.y );
      }

      return result;
    };

    glm::vec2 getMax() {
      glm::vec2 result = { std::numeric_limits< float >::lowest(), std::numeric_limits< float >::lowest() };

      for( const auto& vertex : this->vertices ) {
        result.x = std::max( result.x, vertex.position.x );
        result.y = std::max( result.y, vertex.position.y );
      }

      return result;
    };

  public:
    SquareTextureMapper( std::vector< VertexType >& vertices ) : TextureMapper< VertexType >::TextureMapper( vertices ) {}

    void map() override {
      glm::vec2 min = getMin();
      glm::vec2 max = getMax();
      glm::vec2 span = max - min;

      for( auto& vertex : this->vertices ) {
        vertex.textureCoordinates.s = ( vertex.position.x - min.x ) / span.x;
        vertex.textureCoordinates.t =  1.0f - ( ( vertex.position.y - min.y ) / span.y );
      }
    };
  };

}

#endif
