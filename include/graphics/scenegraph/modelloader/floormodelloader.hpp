#ifndef BB_FLOOR_MODEL_LOADER
#define BB_FLOOR_MODEL_LOADER

#include "graphics/scenegraph/modelloader/proceduralmodelloader.hpp"
#include "models/infrastructure.hpp"
#include "exceptions/genexc.hpp"
#include <memory>

namespace BlueBear {
  namespace Scripting {
    class Tile;
  }

  namespace Graphics {
    namespace SceneGraph {
      class Model;

      namespace ModelLoader {
        class FloorModelLoader : public ProceduralModelLoader {
          const std::vector< Models::Infrastructure::FloorLevel >& levels;
          unsigned int currentLevel = 0;

          sf::Image generateRootTexture( const glm::uvec2& dimensions );

        public:
          FloorModelLoader( const std::vector< Models::Infrastructure::FloorLevel >& levels );
          void setCurrentLevel( unsigned int level );

          virtual std::shared_ptr< Model > get();
        };

      }
    }
  }
}

#endif
