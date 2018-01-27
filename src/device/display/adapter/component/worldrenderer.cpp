#include "device/display/adapter/component/worldrenderer.hpp"
#include "graphics/scenegraph/model.hpp"
#include "graphics/scenegraph/modelloader/filemodelloader.hpp"
#include "graphics/scenegraph/modelloader/assimpmodelloader.hpp"
#include "tools/objectpool.hpp"
#include "tools/utility.hpp"
#include "configmanager.hpp"
#include "log.hpp"
#include <tbb/task_group.h>
#include <tbb/concurrent_queue.h>
#include <functional>

namespace BlueBear {
  namespace Device {
    namespace Display {
      namespace Adapter {
        namespace Component {

          WorldRenderer::WorldRenderer() : camera( Graphics::Camera( ConfigManager::getInstance().getIntValue( "viewport_x" ), ConfigManager::getInstance().getIntValue( "viewport_y" ) ) ) {}

          std::shared_ptr< Graphics::SceneGraph::Model > WorldRenderer::placeObject( const std::string& objectId, const std::string& newId ) {
            auto it = originals.find( objectId );
            if( it == originals.end() ) {
              Log::getInstance().warn( "WorldRenderer::placeObject", std::string( "Warning: Object " ) + objectId + " not loaded or registered!" );
              return nullptr;
            }

            if( models.find( newId ) != models.end() ) {
              Log::getInstance().warn( "WorldRenderer::placeObject", std::string( "Warning: Object with ID " ) + objectId + " already exists; it will be replaced." );
            }

            return models[ newId ] = originals.at( objectId )->copy();
          }

          std::shared_ptr< Graphics::SceneGraph::Model > WorldRenderer::getObject( const std::string& instanceId ) {
            auto it = models.find( instanceId );
            if( it == models.end() ) {
              Log::getInstance().warn( "WorldRenderer::getObject", std::string( "Warning: Instance with ID " ) + instanceId + " doesn't exist!" );
              return nullptr;
            }

            return it->second;
          }

          Graphics::Camera& WorldRenderer::getCamera() {
            return camera;
          }

          std::unique_ptr< Graphics::SceneGraph::ModelLoader::FileModelLoader > WorldRenderer::getFileModelLoader( bool deferGLOperations ) {
            std::unique_ptr< Graphics::SceneGraph::ModelLoader::FileModelLoader > result = std::make_unique< Graphics::SceneGraph::ModelLoader::AssimpModelLoader >();

            Graphics::SceneGraph::ModelLoader::AssimpModelLoader& asAssimp = ( Graphics::SceneGraph::ModelLoader::AssimpModelLoader& )*result;
            asAssimp.deferGLOperations = deferGLOperations;
            asAssimp.cache = &cache;

            return result;
          }

          void WorldRenderer::loadPathsParallel( const std::vector< std::pair< std::string, std::string > >& paths ) {
            tbb::task_group group;
            Tools::ObjectPool< Graphics::SceneGraph::ModelLoader::FileModelLoader > pool( std::bind( &WorldRenderer::getFileModelLoader, this, true ) );

            for( auto& path : paths ) {
              group.run( [ & ]() {
                if( originals.find( path.first ) == originals.end() ) {
                  pool.acquire( [ & ]( Graphics::SceneGraph::ModelLoader::FileModelLoader& loader ) {
                    try {
                      if( std::shared_ptr< Graphics::SceneGraph::Model > model = loader.get( path.second ) ) {
                        originals[ path.first ] = model;
                      }
                    } catch( std::exception& e ) {
                      Log::getInstance().error( "WorldRenderer::loadPathsParallel", std::string( "Could not load model " ) + path.second + ": " + e.what() );
                    }
                  } );
                } else {
                  Log::getInstance().warn( "WorldRenderer::loadPathsParallel", path.first + " is already inserted into this map; skipping" );
                }
              } );
            }

            group.wait();

            for( auto& path : paths ) {
              auto pair = originals.find( path.first );
              if( pair != originals.end() ) {
                pair->second->sendDeferredObjects();
              }
            }
          }

          void WorldRenderer::loadPaths( const std::vector< std::pair< std::string, std::string > >& paths ) {
            std::unique_ptr< Graphics::SceneGraph::ModelLoader::FileModelLoader > loader = getFileModelLoader( false );

            for( auto& path : paths ) {
              try {
                if( std::shared_ptr< Graphics::SceneGraph::Model > model = loader->get( path.second ) ) {
                  originals[ path.first ] = model;
                }
              } catch( std::exception& e ) {
                Log::getInstance().error( "WorldRenderer::loadPathsParallel", std::string( "Could not load model " ) + path.second + ": " + e.what() );
              }
            }
          }

          /**
           * TODO: Optimized renderer that sorts by shader to minimize shader changes
           */
          void WorldRenderer::render() {
            // Position camera
            camera.position();

            // TODO: Position lights
            // TODO: Draw floor and walls
            // Draw models
            for( auto& pair : models ) {
              pair.second->draw();
            }
          }

        }
      }
    }
  }
}
