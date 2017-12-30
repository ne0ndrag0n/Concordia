#ifndef WORLD_RENDERER
#define WORLD_RENDERER

#include "graphics/scenegraph/resourcebank.hpp"
#include <tbb/concurrent_unordered_map.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace BlueBear {
  namespace Graphics {
    class Shader;

    namespace SceneGraph {
      class Model;

      namespace ModelLoader {
        class FileModelLoader;
      }
    }

    namespace World {

      class Renderer {
        SceneGraph::ResourceBank cache;
        tbb::concurrent_unordered_map< std::string, std::shared_ptr< SceneGraph::Model > > originals;

        std::unique_ptr< SceneGraph::ModelLoader::FileModelLoader > getFileModelLoader( bool deferGLOperations );

      public:
        virtual ~Renderer() = default;

        void loadPathsParallel( const std::vector< std::string >& paths );
        void loadPaths( const std::vector< std::string >& paths );
      };

    }
  }
}

#endif
