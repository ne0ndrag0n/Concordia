#ifndef BB_FLOOR_MODEL_LOADER
#define BB_FLOOR_MODEL_LOADER

#include "graphics/scenegraph/modelloader/proceduralmodelloader.hpp"
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

          std::shared_ptr< Model > generateTerrain();

        public:
          EXCEPTION_TYPE( CollectionNotSetException, "Collection not set on FloorModelLoader" );

          FloorModelLoader();

          virtual std::shared_ptr< Model > get();
        };

      }
    }
  }
}

#endif
