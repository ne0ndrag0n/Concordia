#ifndef HOUSE_ENVIRONMENT
#define HOUSE_ENVIRONMENT

#include "containers/collection3d.hpp"
#include "models/environment.hpp"
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

  namespace Scripting {
    class InfrastructureFactory;
  }

  namespace Models {
    class Tile;

    class HouseEnvironment : public Environment {
      Scripting::InfrastructureFactory& infrastructureFactory;
      glm::uvec2 dimensions;
      std::map< double, std::vector< double > > vertices;
      std::map< double, std::vector< std::shared_ptr< Models::Tile > > > tiles;

      void prepElevationPairs( double elevation );
      void prepVertices( const Json::Value& elevationSegment, double elevation );
      void prepTiles(
        const Json::Value& elevationSegment,
        const Json::Value& dict,
        double elevation
      );

    public:
      HouseEnvironment( Scripting::InfrastructureFactory& infrastructureFactory, const Json::Value& lot );

      std::shared_ptr< Graphics::SceneGraph::Model > generateEnvironment();
    };

  }
}


#endif
