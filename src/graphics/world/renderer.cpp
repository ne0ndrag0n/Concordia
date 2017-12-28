#include "graphics/world/renderer.hpp"
#include "graphics/scenegraph/model.hpp"
#include "graphics/scenegraph/modelloader/modelloader.hpp"
#include "graphics/scenegraph/modelloader/assimpmodelloader.hpp"
#include "tools/objectpool.hpp"
#include "log.hpp"
#include <tbb/task_group.h>
#include <tbb/concurrent_queue.h>
#include <tbb/concurrent_vector.h>
#include <functional>
#include <atomic>

namespace BlueBear {
  namespace Graphics {
    namespace World {

      std::unique_ptr< SceneGraph::ModelLoader::FileModelLoader > Renderer::getFileModelLoader( bool deferGLOperations ) const {
        std::unique_ptr< SceneGraph::ModelLoader::FileModelLoader > result = std::make_unique< SceneGraph::ModelLoader::AssimpModelLoader >();

        ( ( SceneGraph::ModelLoader::AssimpModelLoader& ) *result ).deferGLOperations = true;

        return result;
      }

      void Renderer::loadPathsParallel( const std::vector< std::string >& paths ) {
        tbb::task_group group;
        tbb::concurrent_vector< std::shared_ptr< SceneGraph::Model > > sequentials;
        Tools::ObjectPool< SceneGraph::ModelLoader::FileModelLoader > pool( std::bind( &Renderer::getFileModelLoader, this, true ) );

        for( const std::string& path : paths ) {
          group.run( [ & ]() {
            pool.acquire( [ & ]( SceneGraph::ModelLoader::FileModelLoader& loader ) {
              try {
                if( std::shared_ptr< SceneGraph::Model > model = loader.get( path ) ) {
                  sequentials.push_back( originals[ path ] = model );
                }
              } catch( std::exception& e ) {
                Log::getInstance().error( "Renderer::loadPathsParallel", std::string( "Could not load model " ) + path + ": " + e.what() );
              }
            } );
          } );
        }

        group.wait();

        for( std::shared_ptr< SceneGraph::Model > sequential : sequentials ) {
          sequential->sendDeferredObjects();
        }
      }

      void Renderer::loadPaths( const std::vector< std::string >& paths ) {
        std::unique_ptr< SceneGraph::ModelLoader::FileModelLoader > loader = getFileModelLoader( false );

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
