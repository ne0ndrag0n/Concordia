#ifndef WORLD_RENDERER
#define WORLD_RENDERER

#include "containers/vec3map.hpp"
#include "graphics/scenegraph/resourcebank.hpp"
#include <tbb/concurrent_unordered_map.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <utility>

namespace BlueBear {
  namespace Graphics {
    class Shader;

    namespace SceneGraph {
      class Model;
      class Light;

      namespace ModelLoader {
        class FileModelLoader;
      }
    }

    namespace World {

      class Renderer {
        SceneGraph::ResourceBank cache;
        tbb::concurrent_unordered_map< std::string, std::shared_ptr< SceneGraph::Model > > originals;
        Containers::Vec3Map< std::shared_ptr< SceneGraph::Model > > floor;
        Containers::Vec3Map< std::shared_ptr< SceneGraph::Model > > walls;
        std::unordered_map< std::string, std::shared_ptr< SceneGraph::Light > > lights;
        std::unordered_map< std::string, std::shared_ptr< SceneGraph::Model > > models;

        std::unique_ptr< SceneGraph::ModelLoader::FileModelLoader > getFileModelLoader( bool deferGLOperations );

      public:
        virtual ~Renderer() = default;

        void loadPathsParallel( const std::vector< std::pair< std::string, std::string > >& paths );
        void loadPaths( const std::vector< std::pair< std::string, std::string > >& paths );
        void render();
      };

    }
  }
}

#endif
