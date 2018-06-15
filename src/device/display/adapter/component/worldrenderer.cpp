#include "device/display/adapter/component/worldrenderer.hpp"
#include "graphics/scenegraph/light/pointlight.hpp"
#include "graphics/scenegraph/light/directionallight.hpp"
#include "graphics/scenegraph/model.hpp"
#include "graphics/scenegraph/modelloader/filemodelloader.hpp"
#include "graphics/scenegraph/modelloader/assimpmodelloader.hpp"
#include "tools/objectpool.hpp"
#include "tools/utility.hpp"
#include "scripting/entitykit/components/modelmanager.hpp"
#include "scripting/luakit/utility.hpp"
#include "configmanager.hpp"
#include "eventmanager.hpp"
#include "log.hpp"
#include <tbb/task_group.h>
#include <tbb/concurrent_queue.h>
#include <functional>

namespace BlueBear {
  namespace Device {
    namespace Display {
      namespace Adapter {
        namespace Component {

          WorldRenderer::WorldRenderer( Device::Display::Display& display ) :
            Adapter::Adapter( display ),
            camera( Graphics::Camera( ConfigManager::getInstance().getIntValue( "viewport_x" ), ConfigManager::getInstance().getIntValue( "viewport_y" ) ) ) {
              Scripting::EntityKit::Components::ModelManager::worldRenderer = this;
              eventManager.LUA_STATE_READY.listen( this, std::bind( &WorldRenderer::submitLuaContributions, this, std::placeholders::_1 ) );
              eventManager.SHADER_CHANGE.listen( this, std::bind( &WorldRenderer::onShaderChange, this ) );

              lights[ "sun" ] = std::make_shared< Graphics::SceneGraph::Light::DirectionalLight >(
                glm::vec3{ -1.0, 0.0, 0.0 },
                glm::vec3{ 1.0, 1.0, 1.0 },
                glm::vec3{ 0.1, 0.1, 0.1 },
                glm::vec3{ 0.1, 0.1, 0.1 }
              );
            }

          WorldRenderer::~WorldRenderer() {
            eventManager.LUA_STATE_READY.stopListening( this );
            eventManager.SHADER_CHANGE.stopListening( this );
          }

          void WorldRenderer::submitLuaContributions( sol::state& lua ) {
            sol::table world;
            if( lua[ "bluebear" ][ "world" ] == sol::nil ) {
              world = lua[ "bluebear" ][ "world" ] = lua.create_table();
            } else {
              world = lua[ "bluebear" ][ "world" ];
            }

            world.set_function( "register_models", [ & ]( sol::table table ) {
              std::vector< std::pair< std::string, std::string > > models;

              for( std::pair< sol::object, sol::object >& pair : table ) {
                try {
                  models.emplace_back(
                    Scripting::LuaKit::Utility::cast< std::string >( pair.first ),
                    Scripting::LuaKit::Utility::cast< std::string >( pair.second )
                  );
                } catch( Scripting::LuaKit::Utility::InvalidTypeException invalidTypeException ) {
                  Log::getInstance().error( "WorldRenderer::submitLuaContributions", "Argument in table must have format (string id) = (string path)" );
                }
              }

              loadPathsParallel( models );
            } );

            Graphics::SceneGraph::Model::submitLuaContributions( lua );
          }

          std::shared_ptr< Graphics::SceneGraph::Model > WorldRenderer::placeObject( const std::string& objectId, const std::set< std::string >& classes ) {
            auto it = originals.find( objectId );

            if( it != originals.end() ) {
              std::shared_ptr< Graphics::SceneGraph::Model > copy = it->second->copy();
              models.insert( { objectId, classes, copy } );
              return copy;
            } else {
              Log::getInstance().warn( "WorldRenderer::placeObject", "Could not add object of id " + objectId + ": ID not registered!" );
              throw ObjectIDNotRegisteredException();
            }
          }

          std::vector< std::shared_ptr< Graphics::SceneGraph::Model > > WorldRenderer::findObjectsByType( const std::string& instanceId ) {
            std::vector< std::shared_ptr< Graphics::SceneGraph::Model > > result;

            std::for_each( models.begin(), models.end(), [ & ]( const ModelRegistration& item ) {
              if( item.originalId == instanceId ) {
                result.push_back( item.instance );
              }
            } );

            return result;
          }

          std::vector< std::shared_ptr< Graphics::SceneGraph::Model > > WorldRenderer::findObjectsByClass( const std::set< std::string >& classes ) {
            std::vector< std::shared_ptr< Graphics::SceneGraph::Model > > result;

            std::for_each( models.begin(), models.end(), [ & ]( const ModelRegistration& item ) {
              std::set< std::string > difference;

              std::set_symmetric_difference(
                classes.begin(), classes.end(),
                item.instanceClasses.begin(), item.instanceClasses.end(),
                std::inserter( difference, difference.end() )
              );

              if( difference.size() == 0 ) {
                result.push_back( item.instance );
              }
            } );

            return result;
          }

          void WorldRenderer::removeObject( std::shared_ptr< Graphics::SceneGraph::Model > model ) {
            for( auto it = models.begin(); it != models.end(); ) {
              if( it->instance == model ) {
                models.erase( it );
                return;
              } else {
                ++it;
              }
            }
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
            tbb::concurrent_unordered_map< std::string, std::shared_ptr< Graphics::SceneGraph::Model > > concurrentOriginals;
            tbb::task_group group;
            Tools::ObjectPool< Graphics::SceneGraph::ModelLoader::FileModelLoader > pool( std::bind( &WorldRenderer::getFileModelLoader, this, true ) );

            for( auto& path : paths ) {
              group.run( [ & ]() {
                if( concurrentOriginals.find( path.first ) == concurrentOriginals.end() ) {
                  pool.acquire( [ & ]( Graphics::SceneGraph::ModelLoader::FileModelLoader& loader ) {
                    try {
                      if( std::shared_ptr< Graphics::SceneGraph::Model > model = loader.get( path.second ) ) {
                        concurrentOriginals[ path.first ] = model;
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
              auto pair = concurrentOriginals.find( path.first );
              if( pair != concurrentOriginals.end() ) {
                pair->second->sendDeferredObjects();
              }
            }

            // Copy concurrent map to originals
            for( auto& pair : concurrentOriginals ) {
              originals.emplace( pair.first, pair.second );
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

          void WorldRenderer::onShaderChange() {
            Graphics::SceneGraph::Light::PointLight::sendLightCount();
          }

          /**
           * TODO: Optimized renderer that sorts by shader to minimize shader changes
           */
          void WorldRenderer::nextFrame() {
            // Position camera
            camera.position();

            for( auto& registration : models ) {
              registration.instance->draw();
            }
          }

        }
      }
    }
  }
}
