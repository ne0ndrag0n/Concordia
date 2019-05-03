#include "device/display/adapter/component/worldrenderer.hpp"
#include "device/display/display.hpp"
#include "geometry/methods.hpp"
#include "graphics/scenegraph/light/pointlight.hpp"
#include "graphics/scenegraph/light/directionallight.hpp"
#include "graphics/scenegraph/model.hpp"
#include "graphics/scenegraph/modelloader/filemodelloader.hpp"
#include "graphics/scenegraph/modelloader/assimpmodelloader.hpp"
#include "graphics/scenegraph/uniforms/highlight_uniform.hpp"
#include "tools/objectpool.hpp"
#include "tools/utility.hpp"
#include "scripting/luakit/utility.hpp"
#include "configmanager.hpp"
#include "log.hpp"
#include <tbb/task_group.h>
#include <tbb/concurrent_queue.h>
#include <functional>
#include <mutex>

namespace BlueBear {
  namespace Device {
    namespace Display {
      namespace Adapter {
        namespace Component {

          WorldRenderer::WorldRenderer( Device::Display::Display& display ) :
            Adapter::Adapter( display ),
            camera( Graphics::Camera( ConfigManager::getInstance().getIntValue( "viewport_x" ), ConfigManager::getInstance().getIntValue( "viewport_y" ) ) ) {
              eventManager.LUA_STATE_READY.listen( this, std::bind( &WorldRenderer::submitLuaContributions, this, std::placeholders::_1 ) );
              eventManager.SHADER_CHANGE.listen( this, std::bind( &WorldRenderer::onShaderChange, this ) );

              asyncTasks.setAmountPerFrame( 333 );
            }

          WorldRenderer::~WorldRenderer() {
            eventManager.LUA_STATE_READY.stopListening( this );
            eventManager.SHADER_CHANGE.stopListening( this );
          }

          Json::Value WorldRenderer::save() {

          }

          void WorldRenderer::load( const Json::Value& data ) {
            if( data != Json::Value::null ) {
              // Load illuminators
              const Json::Value& savedLights = data[ "illuminators" ];

              for( auto it = savedLights.begin(); it != savedLights.end(); ++it ) {
                auto pair = Tools::Utility::jsonIteratorToPair( it );
                const Json::Value& lightDefinition = pair.second.get();

                switch( Tools::Utility::hash( lightDefinition[ "type" ].asCString() ) ) {
                  case Tools::Utility::hash( "directionalLight" ): {
                    illuminators[ pair.first ] = std::make_shared< Graphics::SceneGraph::Light::DirectionalLight >(
                      glm::vec3{ lightDefinition[ "direction" ][ 0 ].asDouble(), lightDefinition[ "direction" ][ 1 ].asDouble(), lightDefinition[ "direction" ][ 2 ].asDouble() },
                      glm::vec3{ lightDefinition[ "ambient" ][ 0 ].asDouble(), lightDefinition[ "ambient" ][ 1 ].asDouble(), lightDefinition[ "ambient" ][ 2 ].asDouble() },
                      glm::vec3{ lightDefinition[ "diffuse" ][ 0 ].asDouble(), lightDefinition[ "diffuse" ][ 1 ].asDouble(), lightDefinition[ "diffuse" ][ 2 ].asDouble() },
                      glm::vec3{ lightDefinition[ "specular" ][ 0 ].asDouble(), lightDefinition[ "specular" ][ 1 ].asDouble(), lightDefinition[ "specular" ][ 2 ].asDouble() }
                    );
                    break;
                  }
                  default:
                    Log::getInstance().error( "WorldRenderer::load", "Invalid light type specified: " + lightDefinition[ "type" ].asString() + " (valid types are: directionalLight)" );
                }
              }
            }
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

          void WorldRenderer::registerEvents( Device::Input::Input& inputManager ) {
            inputManager.registerInputEvent(
              sf::Event::MouseButtonPressed,
              std::bind( &BlueBear::Device::Display::Adapter::Component::WorldRenderer::onMouseDown, this, std::placeholders::_1 )
            );

            inputManager.registerInputEvent(
              sf::Event::MouseButtonReleased,
              std::bind( &BlueBear::Device::Display::Adapter::Component::WorldRenderer::onMouseUp, this, std::placeholders::_1 )
            );

            inputManager.registerInputEvent(
              sf::Event::MouseMoved,
              std::bind( &BlueBear::Device::Display::Adapter::Component::WorldRenderer::onMouseMoved, this, std::placeholders::_1 )
            );
          }

          void WorldRenderer::fireInOutEvents( const ModelRegistration* selected, const Device::Input::Metadata& event ) {
            // Nothing changes if selected is the same as previousMove
            if( selected != previousMove ) {
              // Fire mouse-out event on previousMove
              if( previousMove ) {
                auto it = previousMove->events.find( "mouse-out" );
                if( it != previousMove->events.end() ) {
                  for( const auto& f : it->second ) {
                    if( f ) { f( event, previousMove->instance ); }
                  }
                }
              }

              previousMove = selected;

              // Fire mouse-in event on selected item
              if( selected ) {
                auto it = selected->events.find( "mouse-in" );
                if( it != selected->events.end() ) {
                  for( const auto& f : it->second ) {
                    if( f ) { f( event, selected->instance ); }
                  }
                }
              }
            }
          }

          void WorldRenderer::onMouseDown( Device::Input::Metadata metadata ) {
            Geometry::Ray ray = camera.getPickingRay( metadata.mouseLocation, display.getDimensions() );

            std::vector< const ModelRegistration* > candidates = narrowByEvent( { "mouse-down" } );
            std::vector< const ModelRegistration* > relevant;
            for( const ModelRegistration* registration : candidates ) {
              if( registration->instance->intersectsBoundingVolume( ray ) ) {
                relevant.emplace_back( registration );
              }
            }

            getModelAtMouse( ray, relevant, [ this, metadata ]( const ModelRegistration* found ) {
              if( found ) {
                auto it = found->events.find( "mouse-down" );
                if( it != found->events.end() ) {
                  for( const auto& f : it->second ) {
                    if( f ) { f( metadata, found->instance ); }
                  }
                }
              }
            } );
          }

          void WorldRenderer::onMouseUp( Device::Input::Metadata metadata ) {
            Geometry::Ray ray = camera.getPickingRay( metadata.mouseLocation, display.getDimensions() );

            std::vector< const ModelRegistration* > candidates = narrowByEvent( { "mouse-up" } );
            std::vector< const ModelRegistration* > relevant;
            for( const ModelRegistration* registration : candidates ) {
              if( registration->instance->intersectsBoundingVolume( ray ) ) {
                relevant.emplace_back( registration );
              }
            }

            getModelAtMouse( ray, relevant, [ this, metadata ]( const ModelRegistration* found ) {
              if( found ) {
                auto it = found->events.find( "mouse-up" );
                if( it != found->events.end() ) {
                  for( const auto& f : it->second ) {
                    if( f ) { f( metadata, found->instance ); }
                  }
                }
              }
            } );
          }

          void WorldRenderer::onMouseMoved( Device::Input::Metadata metadata ) {
            static bool inProgress = false;
            if( inProgress ) {
              return;
            }

            inProgress = true;

            std::vector< const ModelRegistration* > candidates = narrowByEvent( { "mouse-in", "mouse-out" } );

            Geometry::Ray ray = camera.getPickingRay( metadata.mouseLocation, display.getDimensions() );

            // Create a "relevant" list only for items which intersect the bounding volume
            std::vector< const ModelRegistration* > relevant;
            for( const ModelRegistration* registration : candidates ) {
              if( registration->instance->intersectsBoundingVolume( ray ) ) {
                relevant.emplace_back( registration );
              }
            }

            getModelAtMouse( ray, relevant, [ this, metadata ]( const ModelRegistration* found ) {
              fireInOutEvents( found, metadata );
              inProgress = false;
            } );
          }

          std::vector< const WorldRenderer::ModelRegistration* > WorldRenderer::narrowByEvent( const std::set< std::string >& acceptable ) {
            std::vector< const ModelRegistration* > result;

            for( const auto& registration : models ) {
              if( registration ) {
                for( const auto& event : registration->events ) {
                  if( acceptable.find( event.first ) != acceptable.end() ) {
                    result.emplace_back( registration.get() );
                    break;
                  }
                }
              }
            }

            return result;
          }

          void WorldRenderer::getModelAtMouse(
            const Geometry::Ray& ray,
            const std::vector< const ModelRegistration* >& candidateModels,
            const std::function< void( const ModelRegistration* ) >& callback
          ) {
            struct ModelTriangle {
              std::pair< Geometry::Triangle, glm::mat4 > modelTriangle;
              const ModelRegistration* registration;
            };

            std::vector< ModelTriangle > modelTriangles;
            for( const ModelRegistration* registration : candidateModels ) {
              auto trianglePairs = registration->instance->getModelTriangles();
              // Unzip triangles
              for( const auto& trianglePair : trianglePairs ) {
                modelTriangles.emplace_back( ModelTriangle{ trianglePair, registration } );
              }
            }

            struct HeapObject {
              const ModelRegistration* result = nullptr;
              float lastDistance = std::numeric_limits< float >::max();
            };
            std::shared_ptr< HeapObject > heap = std::make_shared< HeapObject >();

            std::vector< Tools::AsyncTable::Task > tasks;
            for( const auto& candidate : modelTriangles ) {
              tasks.emplace_back( [ heap, ray, candidate ]() {
                Geometry::Triangle triangle{
                  candidate.modelTriangle.second * glm::vec4{ candidate.modelTriangle.first[ 0 ], 1.0f },
                  candidate.modelTriangle.second * glm::vec4{ candidate.modelTriangle.first[ 1 ], 1.0f },
                  candidate.modelTriangle.second * glm::vec4{ candidate.modelTriangle.first[ 2 ], 1.0f }
                };

                if( auto potentialIntersection = Geometry::getIntersectionPoint( ray, triangle ) ) {
                  float distance = Tools::Utility::distance( ray.origin, *potentialIntersection );
                  if( distance < heap->lastDistance ) {
                    heap->lastDistance = distance;
                    heap->result = candidate.registration;
                  }
                }
              } );
            }

            asyncTasks.enqueue( tasks, [ heap, callback ]() {
              if( callback ) {
                callback( heap->result );
              }
            } );
          }

          std::shared_ptr< Graphics::SceneGraph::Model > WorldRenderer::placeObject( const std::string& objectId, const std::set< std::string >& classes ) {
            auto it = originals.find( objectId );

            if( it != originals.end() ) {
              std::shared_ptr< Graphics::SceneGraph::Model > copy = it->second->copy();

              std::unique_ptr< ModelRegistration > registration = std::make_unique< ModelRegistration >( ModelRegistration{ objectId, classes, copy } );

              models.emplace_back( std::move( registration ) );
              MODEL_ADDED.trigger( copy );

              return copy;
            } else {
              Log::getInstance().warn( "WorldRenderer::placeObject", "Could not add object of id " + objectId + ": ID not registered!" );
              throw ObjectIDNotRegisteredException();
            }
          }

          std::vector< std::shared_ptr< Graphics::SceneGraph::Model > > WorldRenderer::findObjectsByType( const std::string& instanceId ) {
            std::vector< std::shared_ptr< Graphics::SceneGraph::Model > > result;

            std::for_each( models.begin(), models.end(), [ & ]( const std::unique_ptr< ModelRegistration >& item ) {
              if( item && item->originalId == instanceId ) {
                result.push_back( item->instance );
              }
            } );

            return result;
          }

          std::vector< std::shared_ptr< Graphics::SceneGraph::Model > > WorldRenderer::findObjectsByClass( const std::set< std::string >& classes ) {
            std::vector< std::shared_ptr< Graphics::SceneGraph::Model > > result;

            std::for_each( models.begin(), models.end(), [ & ]( const std::unique_ptr< ModelRegistration >& item ) {
              if( item ) {
                std::set< std::string > difference;

                std::set_symmetric_difference(
                  classes.begin(), classes.end(),
                  item->instanceClasses.begin(), item->instanceClasses.end(),
                  std::inserter( difference, difference.end() )
                );

                if( difference.size() == 0 ) {
                  result.push_back( item->instance );
                }
              }
            } );

            return result;
          }

          std::vector< std::shared_ptr< Graphics::SceneGraph::Illuminator > > WorldRenderer::findIlluminators( const std::regex& regex ) {
            std::vector< std::shared_ptr< Graphics::SceneGraph::Illuminator > > result;

            for( const auto& pair : illuminators ) {
              if( std::regex_match( pair.first, regex ) ) {
                result.push_back( pair.second );
              }
            }

            return result;
          }

          std::shared_ptr< Graphics::SceneGraph::Illuminator > WorldRenderer::getIlluminator( const std::string& id ) {
            auto it = illuminators.find( id );
            if( it != illuminators.end() ) {
              return it->second;
            }

            return nullptr;
          }

          void WorldRenderer::addIlluminator( const std::string& id, std::shared_ptr< Graphics::SceneGraph::Illuminator > illuminator ) {
            illuminators.emplace( id, illuminator );
          }

          void WorldRenderer::removeIlluminator( const std::string& id ) {
            illuminators.erase( id );
          }

          void WorldRenderer::removeObject( std::shared_ptr< Graphics::SceneGraph::Model > model ) {
            for( auto it = models.begin(); it != models.end(); ) {
              if( ( *it )->instance == model ) {
                models.erase( it );
                MODEL_REMOVED.trigger( model );
                return;
              } else {
                ++it;
              }
            }
          }

          unsigned int WorldRenderer::registerEvent(
            std::shared_ptr< Graphics::SceneGraph::Model > instance,
            const std::string& eventTag,
            const ModelEventCallback& callback
          ) {
            ModelRegistration* registration = nullptr;
            for( const auto& modelRegistration : models ) {
              if( modelRegistration && modelRegistration->instance == instance ) {
                registration = modelRegistration.get();
                break;
              }
            }

            if( registration ) {
              auto& bucket = registration->events[ eventTag ];
              bool found = false;
              for( auto& item : bucket ) {
                if( !item ) {
                  found = true;
                  item = callback;
                  break;
                }
              }

              if( !found ) {
                bucket.push_back( callback );
              }
            } else {
              Log::getInstance().warn( "WorldRenderer::registerEvent", "Instance not registered" );
            }
          }

          void WorldRenderer::unregisterEvent( std::shared_ptr< Graphics::SceneGraph::Model > instance, const std::string& eventTag, unsigned int item ) {
            ModelRegistration* registration = nullptr;
            for( const auto& modelRegistration : models ) {
              if( modelRegistration && modelRegistration->instance == instance ) {
                registration = modelRegistration.get();
                break;
              }
            }

            if( registration ) {
              registration->events[ eventTag ][ item ] = {};
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

          void WorldRenderer::loadDirect( const std::string& id, const std::shared_ptr< Graphics::SceneGraph::Model >& model ) {
            auto it = originals.find( id );
            if( it != originals.end() ) {
              Log::getInstance().warn( "WorldRenderer::loadDirect", "Overloading original registered model of type " + id );
            }

            originals[ id ] = model;
          }

          void WorldRenderer::onShaderChange() {
            Graphics::SceneGraph::Light::PointLight::sendLightCount();
          }

          /**
           * TODO: Optimized renderer that sorts by shader to minimize shader changes
           */
          void WorldRenderer::nextFrame() {
            asyncTasks.update();

            // Position camera
            camera.position();

            for( auto& registration : models ) {
              if( registration ) {
                registration->instance->draw();
              }
            }
          }

        }
      }
    }
  }
}
