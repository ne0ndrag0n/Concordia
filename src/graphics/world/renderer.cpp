#include "graphics/world/renderer.hpp"
#include "graphics/scenegraph/model.hpp"
#include "graphics/scenegraph/modelloader/modelloader.hpp"
#include "graphics/scenegraph/modelloader/assimpmodelloader.hpp"
#include "tools/objectpool.hpp"
#include "tools/utility.hpp"
#include "log.hpp"
#include <tbb/task_group.h>
#include <tbb/concurrent_queue.h>
#include <functional>

namespace BlueBear {
  namespace Graphics {
    namespace World {

      std::unique_ptr< SceneGraph::ModelLoader::FileModelLoader > Renderer::getFileModelLoader( bool deferGLOperations ) {
        std::unique_ptr< SceneGraph::ModelLoader::FileModelLoader > result = std::make_unique< SceneGraph::ModelLoader::AssimpModelLoader >();

        SceneGraph::ModelLoader::AssimpModelLoader& asAssimp = ( SceneGraph::ModelLoader::AssimpModelLoader& )*result;
        asAssimp.deferGLOperations = true;
        asAssimp.cache = &cache;

        return result;
      }

      void Renderer::loadPathsParallel( const std::vector< std::string >& paths ) {
        tbb::task_group group;
        Tools::ObjectPool< SceneGraph::ModelLoader::FileModelLoader > pool( std::bind( &Renderer::getFileModelLoader, this, true ) );

        for( const std::string& path : paths ) {
          group.run( [ & ]() {
            if( originals.find( path ) == originals.end() ) {
              pool.acquire( [ & ]( SceneGraph::ModelLoader::FileModelLoader& loader ) {
                try {
                  if( std::shared_ptr< SceneGraph::Model > model = loader.get( path ) ) {
                    originals[ path ] = model;
                  }
                } catch( std::exception& e ) {
                  Log::getInstance().error( "Renderer::loadPathsParallel", std::string( "Could not load model " ) + path + ": " + e.what() );
                }
              } );
            } else {
              Log::getInstance().warn( "Renderer::loadPathsParallel", path + " is already inserted in this map; skipping" );
            }
          } );
        }

        group.wait();

        for( const std::string& path : paths ) {
          auto pair = originals.find( path );
          if( pair != originals.end() ) {
            pair->second->sendDeferredObjects();
          }
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
