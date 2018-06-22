#ifndef BB_FLOOR_MODEL_LOADER
#define BB_FLOOR_MODEL_LOADER

#include "graphics/scenegraph/modelloader/proceduralmodelloader.hpp"
#include "graphics/scenegraph/mesh/texturedvertex.hpp"
#include "graphics/scenegraph/mesh/meshdefinition.hpp"
#include "models/infrastructure.hpp"
#include "exceptions/genexc.hpp"
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <array>

namespace BlueBear {
  namespace Models {
    class FloorTile;
  }

  namespace Scripting {
    class Tile;
  }

  namespace Graphics {
    namespace SceneGraph {
      class Model;

      namespace ModelLoader {
        class FloorModelLoader : public ProceduralModelLoader {
          const std::vector< Models::Infrastructure::FloorLevel >& levels;

          sf::Image generateTexture( const Models::Infrastructure::FloorLevel& currentLevel );
          glm::vec2 getTextureCoords( const glm::vec2& vertexCoord, const glm::vec2& boundaries );

        public:
          FloorModelLoader( const std::vector< Models::Infrastructure::FloorLevel >& levels );

          virtual std::shared_ptr< Model > get();
        };

      }
    }
  }
}

#endif
