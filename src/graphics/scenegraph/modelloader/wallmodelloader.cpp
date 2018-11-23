#include "graphics/scenegraph/modelloader/wallmodelloader.hpp"
#include "graphics/scenegraph/mesh/facemeshgenerator.hpp"

namespace BlueBear::Graphics::SceneGraph::ModelLoader {

  WallModelLoader::WallModelLoader( const std::vector< Models::WallSegment >& segments ) : segments( segments ) {}

  std::shared_ptr< Model > WallModelLoader::get() {
    // Generate joint map to check for intersections
    // Walk each line
    // Check joint map to see if vertices need to be stretched to close corners

    return nullptr;
  }

}
