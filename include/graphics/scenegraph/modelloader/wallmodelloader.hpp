#ifndef BB_WALL_MODEL_LOADER
#define BB_WALL_MODEL_LOADER

#include "graphics/scenegraph/modelloader/proceduralmodelloader.hpp"
#include "models/infrastructure.hpp"
#include <SFML/Graphics/Image.hpp>
#include <vector>
#include <memory>

namespace BlueBear::Graphics::SceneGraph { class Model; }
namespace BlueBear::Graphics::SceneGraph::ModelLoader {

  class WallModelLoader : public ProceduralModelLoader {
    const std::vector< Models::Infrastructure::FloorLevel >& levels;

    sf::Image generateTexture( const Models::Infrastructure::FloorLevel& currentLevel );

  public:
    WallModelLoader( const std::vector< Models::Infrastructure::FloorLevel >& levels );

    std::shared_ptr< Model > get() override;
  };

}

#endif
