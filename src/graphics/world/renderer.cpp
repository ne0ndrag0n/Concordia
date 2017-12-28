#include "graphics/world/renderer.hpp"
#include "graphics/scenegraph/modelloader/modelloader.hpp"
#include "graphics/scenegraph/modelloader/assimpmodelloader.hpp"
#include "tools/objectpool.hpp"
#include "log.hpp"
#include <tbb/task_group.h>
#include <tbb/concurrent_queue.h>
#include <functional>
#include <atomic>

namespace BlueBear {
  namespace Graphics {
    namespace World {

      std::unique_ptr< SceneGraph::ModelLoader::FileModelLoader > Renderer::getFileModelLoader() const {
        return std::make_unique< SceneGraph::ModelLoader::AssimpModelLoader >();
      }

      void Renderer::loadPathsParallel( const std::vector< std::string >& paths ) {
        tbb::task_group group;
        Tools::ObjectPool< SceneGraph::ModelLoader::FileModelLoader > pool( std::bind( &Renderer::getFileModelLoader, this ) );

        for( const std::string& path : paths ) {
          group.run( [ & ]() {
            pool.acquire( [ & ]( SceneGraph::ModelLoader::FileModelLoader& loader ) {
              try {
                if( std::shared_ptr< SceneGraph::Model > model = loader.get( path ) ) {
                  originals[ path ] = model;
                }
              } catch( std::exception& e ) {
                Log::getInstance().error( "Renderer::loadPathsParallel", std::string( "Could not load model " ) + path + ": " + e.what() );
              }
            } );
          } );
        }

        group.wait();
      }

      void Renderer::loadPaths( const std::vector< std::string >& paths ) {
        std::unique_ptr< SceneGraph::ModelLoader::FileModelLoader > loader = getFileModelLoader();

        for( const std::string& path : paths ) {
          try {
            if( std::shared_ptr< SceneGraph::Model > model = loader->get( path ) ) {
              originals[ path ] = model;
            }
          } catch( std::exception& e ) {
            Log::getInstance().error( "Renderer::loadPathsParallel", std::string( "Could not load model " ) + path + ": " + e.what() );
          }
        }
      }

    }
  }
}
