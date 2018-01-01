#ifndef SG_MODEL_LOADER_BASE
#define SG_MODEL_LOADER_BASE

#include "graphics/scenegraph/modelloader/modelloader.hpp"
#include <memory>
#include <string>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      class Model;

      namespace ModelLoader {

        class FileModelLoader : public ModelLoader {
        public:
          virtual std::shared_ptr< Model > get( const std::string& filename ) = 0;
        };

      }
    }
  }
}

#endif
