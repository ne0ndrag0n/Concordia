#ifndef HOUSE_ENVIRONMENT
#define HOUSE_ENVIRONMENT

#include "containers/collection3d.hpp"
#include "models/environment.hpp"
#include "models/tile.hpp"
#include <glm/glm.hpp>
#include <jsoncpp/json/value.h>
#include <memory>
#include <map>
#include <vector>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      class Model;
    }
  }

  namespace Models {

    class HouseEnvironment : public Environment {
      glm::uvec2 dimensions;
      std::map< double, std::vector< double > > vertices;
      std::map< double, std::vector< const Tile* > > tiles;
      std::vector< std::unique_ptr< Tile > > tileStore;

      Tile* getOrCreateTile( const std::string& id, const std::string& path );
      void prepElevationPairs( double elevation );
      void prepVertices( const Json::Value& elevationSegment, double elevation );
      void prepTiles(
        const Json::Value& elevationSegment,
        const Json::Value& dict,
        double elevation
      );

    public:
      HouseEnvironment( const Json::Value& lot );

      std::shared_ptr< Graphics::SceneGraph::Model > generateEnvironment();
    };

  }
}


#endif
