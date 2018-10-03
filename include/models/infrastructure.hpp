#ifndef INFRASTRUCTURE
#define INFRASTRUCTURE

#include "exceptions/genexc.hpp"
#include "models/utilities/worldcache.hpp"
#include "models/floortile.hpp"
#include "models/walljoint.hpp"
#include "models/wallpaperregion.hpp"
#include "serializable.hpp"
#include <jsoncpp/json/json.h>
#include <glm/glm.hpp>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <map>

namespace BlueBear::Device::Display::Adapter::Component{ class WorldRenderer; }
namespace BlueBear::Graphics::Vector{ class Renderer; }
namespace BlueBear::Graphics::SceneGraph{ class Model; }
namespace BlueBear::Models {

  class Infrastructure : public Serializable {
  public:
    struct FloorLevel {
      glm::uvec2 dimensions;
      std::vector< std::vector< std::optional< FloorTile > > > tiles;
      std::vector< std::vector< float > > vertices;
    };

  private:
    EXCEPTION_TYPE( InvalidImageException, "Invalid image path!" );

    std::vector< FloorLevel > levels;

  public:
    Json::Value save() override;
    void load( const Json::Value& data ) override;
    void load(
      const Json::Value& data,
      Device::Display::Adapter::Component::WorldRenderer& worldRenderer,
      Graphics::Vector::Renderer& renderer,
      Utilities::WorldCache& worldCache
    );
  };

}

#endif
