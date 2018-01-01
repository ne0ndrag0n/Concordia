#ifndef BB_PROCEDURAL_MODEL_LOADER
#define BB_PROCEDURAL_MODEL_LOADER

#include "graphics/scenegraph/modelloader/modelloader.hpp"
#include <memory>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      class Model;

      namespace ModelLoader {

        class ProceduralModelLoader : public ModelLoader {
        public:
          virtual std::shared_ptr< Model > get() = 0;
        };

      }
    }
  }
}

#endif
