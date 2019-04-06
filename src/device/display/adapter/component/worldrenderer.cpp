#include "device/display/adapter/component/worldrenderer.hpp"
#include "device/display/display.hpp"
#include "geometry/triangle.hpp"
#include "geometry/methods.hpp"
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
#include <mutex>

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
          }

          Geometry::Ray WorldRenderer::getRayFromMouseEvent( const glm::ivec2& mouseLocation ) {
            // origin, direction
            Geometry::Ray ray;

            const glm::uvec2& screenDimensions = display.getDimensions();
            glm::mat4 viewMatrix = camera.getOrthoView();
            glm::mat4 projectionMatrix = camera.getOrthoMatrix();

            glm::ivec2 mouseCoords( mouseLocation.x, screenDimensions.y - mouseLocation.y );

            glm::vec3 nearPlane = glm::unProject(
              glm::vec3( mouseCoords.x, mouseCoords.y, 0.0f ),
              viewMatrix,
              projectionMatrix,
              glm::vec4( 0.0f, 0.0f, screenDimensions.x, screenDimensions.y )
            );

            glm::vec3 farPlane = glm::unProject(
              glm::vec3( mouseCoords.x, mouseCoords.y, 1.0f ),
              viewMatrix,
              projectionMatrix,
              glm::vec4( 0.0f, 0.0f, screenDimensions.x, screenDimensions.y )
            );

            ray.origin = nearPlane;
            ray.direction = glm::normalize( farPlane - nearPlane );

            // send Ray off to do his job, he's a good guy
            return ray;
          }

          void WorldRenderer::onMouseDown( Device::Input::Metadata metadata ) {
            // Go bother Ray
            Geometry::Ray ray = getRayFromMouseEvent( metadata.mouseLocation );

            struct IntersectionCandidate {
              Geometry::Triangle triangle;
              glm::mat4 modelTransform;
              const ModelRegistration* associatedRegistration;
            };

            // Build list of IntersectionCandidates
            std::vector< IntersectionCandidate > intersectionCandidates;
            for( const auto& registration : models ) {
              //if( registration.events.find( "mouse-down" ) != registration.events.end() ) {
                auto modelTriangles = registration.instance->getModelTriangles();
                for( const auto& triangle : modelTriangles ) {
                  intersectionCandidates.emplace_back( IntersectionCandidate{ triangle.first, triangle.second, &registration } );
                }
              //}
            }

            // Compare ray to each triangle intersection
            std::mutex mutex;
            const ModelRegistration* closestIntersectingModel = nullptr;
            float lastDistance = std::numeric_limits< float >::max();

            Tools::Utility::runParallel< IntersectionCandidate >( intersectionCandidates, [ & ]( const IntersectionCandidate& candidate ) {
              // TODO: Triangle transform must take into account current state of bone animation
              Geometry::Triangle triangle{
                candidate.modelTransform * glm::vec4{ candidate.triangle[ 0 ], 0.0f },
                candidate.modelTransform * glm::vec4{ candidate.triangle[ 1 ], 0.0f },
                candidate.modelTransform * glm::vec4{ candidate.triangle[ 2 ], 0.0f }
              };

              if( auto potentialIntersection = Geometry::getIntersectionPoint( ray, triangle ) ) {
                float distance = Tools::Utility::distance( ray.origin, *potentialIntersection );
                {
                  std::unique_lock< std::mutex > lock( mutex );
                  if( distance < lastDistance ) {
                    lastDistance = distance;
                    closestIntersectingModel = candidate.associatedRegistration;
                  }
                }
              }
            } );

            // TODO: remove this DEBUG code and replace with firing of events
            if( closestIntersectingModel ) {
              Log::getInstance().debug( "WorldRenderer::onMouseDown", "Intersection found model ID is " + closestIntersectingModel->instance->getId() );
            } else {
              Log::getInstance().debug( "WorldRenderer::onMouseDown", "No intersection at this point" );
            }
          }

          void WorldRenderer::onMouseUp( Device::Input::Metadata metadata ) {
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
