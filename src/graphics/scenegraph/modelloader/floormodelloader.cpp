#include "graphics/scenegraph/modelloader/floormodelloader.hpp"
#include "graphics/scenegraph/model.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      namespace ModelLoader {

        FloorModelLoader::FloorModelLoader() {}

        std::shared_ptr< Model > FloorModelLoader::generateTerrain() {

        }

        std::shared_ptr< Model > FloorModelLoader::get() {
          if( !floor ) {
            throw CollectionNotSetException();
          }


        }

      }
    }
  }
}
