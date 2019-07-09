#ifndef WORLD_RENDERER
#define WORLD_RENDERER

#include "containers/reusableobjectvector.hpp"
#include "device/input/input.hpp"
#include "device/display/adapter/adapter.hpp"
#include "geometry/ray.hpp"
#include "geometry/triangle.hpp"
#include "graphics/shader.hpp"
#include "graphics/camera.hpp"
#include "graphics/uniform_cache.hpp"
#include "graphics/scenegraph/resourcebank.hpp"
#include "graphics/utilities/shader_manager.hpp"
#include "exceptions/genexc.hpp"
#include "tools/async_table.hpp"
#include "eventmanager.hpp"
#include "serializable.hpp"
#include <glm/glm.hpp>
#include <sol.hpp>
#include <memory>
#include <string>
#include <map>
#include <unordered_map>
#include <set>
#include <vector>
#include <utility>
#include <regex>
#include <optional>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      class Model;
      class Illuminator;

      namespace Animation {
        class Animator;
      }

      namespace ModelLoader {
        class FileModelLoader;
      }
    }
  }

  namespace Device {
    namespace Display {
      class Display;

      namespace Adapter {
        namespace Component {

          class WorldRenderer : public Adapter, public Serializable {
          public:
            using ModelEventCallback = std::function< void( Device::Input::Metadata, std::shared_ptr< Graphics::SceneGraph::Model > ) >;
            BasicEvent< void*, std::shared_ptr< Graphics::SceneGraph::Model > > MODEL_ADDED;
            BasicEvent< void*, std::shared_ptr< Graphics::SceneGraph::Model > > MODEL_REMOVED;

          private:
            struct ModelPushdown {
              const std::map< std::string, glm::mat4 >* bones = nullptr;
              glm::mat4 levelTransform;
            };

            struct ModelRegistration {
              const std::string originalId;
              const std::set< std::string > instanceClasses;
              std::shared_ptr< Graphics::SceneGraph::Model > instance;
              std::unordered_map< std::string, std::vector< ModelEventCallback > > events;

              bool operator<( const ModelRegistration& rhs ) const {
                return instance.get() < rhs.instance.get();
              };
            };

            Graphics::Camera camera;
            Graphics::Utilities::ShaderManager& shaderManager;
            Graphics::SceneGraph::ResourceBank cache;
            const ModelRegistration* previousMove = nullptr;
            std::unordered_map< std::string, std::shared_ptr< Graphics::SceneGraph::Model > > originals;
            std::vector< std::unique_ptr< ModelRegistration > > models;
            Tools::AsyncTable asyncTasks;

            struct ModelUniforms {
              Graphics::Shader::Uniform transformUniform;

              ModelUniforms() = default;
              ModelUniforms( const Graphics::Shader& shader ) : transformUniform( shader.getUniform( "model" ) ) {}
            };
            Graphics::UniformCache< ModelUniforms > modelUniforms;

            std::unique_ptr< Graphics::SceneGraph::ModelLoader::FileModelLoader > getFileModelLoader( bool deferGLOperations );

            void getModelAtMouse(
              const Geometry::Ray& ray,
              const std::vector< const ModelRegistration* >& candidateModels,
              const std::function< void( const ModelRegistration* ) >& callback
            );

            void fireInOutEvents( const ModelRegistration* selected, const Device::Input::Metadata& event );

            void onMouseDown( Device::Input::Metadata metadata );
            void onMouseUp( Device::Input::Metadata metadata );
            void onMouseMoved( Device::Input::Metadata metadata );

            std::vector< const ModelRegistration* > getModels();

          public:
            EXCEPTION_TYPE( ObjectIDNotRegisteredException, "Object ID not registered!" );
            WorldRenderer( Display& display, Graphics::Utilities::ShaderManager& shaderManager );
            virtual ~WorldRenderer();

            Json::Value save() override;
            void load( const Json::Value& data ) override;

            void submitLuaContributions( sol::state& lua );

            void registerEvents( Device::Input::Input& inputManager );

            std::shared_ptr< Graphics::SceneGraph::Model > placeObject( const std::string& objectId, const std::set< std::string >& classes = {} );
            std::vector< std::shared_ptr< Graphics::SceneGraph::Model > > findObjectsByType( const std::string& instanceId );
            std::vector< std::shared_ptr< Graphics::SceneGraph::Model > > findObjectsByClass( const std::set< std::string >& classes );
            void removeObject( std::shared_ptr< Graphics::SceneGraph::Model > model );

            std::optional< unsigned int > registerEvent(
              std::shared_ptr< Graphics::SceneGraph::Model > instance,
              const std::string& eventTag,
              const ModelEventCallback& callback
            );
            void unregisterEvent(
              std::shared_ptr< Graphics::SceneGraph::Model > instance,
              const std::string& eventTag,
              unsigned int item
            );

            Graphics::Camera& getCamera();
            void loadPathsParallel( const std::vector< std::pair< std::string, std::string > >& paths );
            void loadPaths( const std::vector< std::pair< std::string, std::string > >& paths );
            void loadDirect( const std::string& id, const std::shared_ptr< Graphics::SceneGraph::Model >& model );
            void drawTree( Graphics::SceneGraph::Model* model, ModelPushdown pushdown );
            void nextFrame() override;
          };

        }
      }
    }
  }

}

#endif
