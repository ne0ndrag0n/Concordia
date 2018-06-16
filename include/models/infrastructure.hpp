#ifndef INFRASTRUCTURE
#define INFRASTRUCTURE

#include "exceptions/genexc.hpp"
#include "models/floortile.hpp"
#include "serializable.hpp"
#include <SFML/Graphics/Image.hpp>
#include <jsoncpp/json/json.h>
#include <glm/glm.hpp>
#include <memory>
#include <optional>
#include <string>
#include <map>

namespace BlueBear::Device::Display::Adapter::Component{ class WorldRenderer; }
namespace BlueBear::Graphics::SceneGraph{ class Model; }
namespace BlueBear::Models {

  class Infrastructure : public Serializable {
    EXCEPTION_TYPE( InvalidImageException, "Invalid image path!" );

    Device::Display::Adapter::Component::WorldRenderer& worldRenderer;
    glm::uvec3 floorDimensions;
    std::map< unsigned int, std::map< unsigned int, std::map< unsigned int, std::optional< FloorTile > > > > floorSurface;
    std::map< std::string, FloorTile > originalTiles;
    std::map< std::string, std::shared_ptr< sf::Image > > images;

    std::shared_ptr< sf::Image > loadImage( const std::string& path );

  public:
    Infrastructure( Device::Display::Adapter::Component::WorldRenderer& worldRenderer );

    Json::Value save() override;
    void load( const Json::Value& data ) override;
  };

}

#endif
