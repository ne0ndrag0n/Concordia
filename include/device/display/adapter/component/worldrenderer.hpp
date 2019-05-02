#ifndef WORLD_RENDERER
#define WORLD_RENDERER

#include "containers/reusableobjectvector.hpp"
#include "device/input/input.hpp"
#include "device/display/adapter/adapter.hpp"
#include "geometry/ray.hpp"
#include "geometry/triangle.hpp"
#include "graphics/camera.hpp"
#include "graphics/scenegraph/resourcebank.hpp"
#include "exceptions/genexc.hpp"
#include "tools/async_table.hpp"
#include "eventmanager.hpp"
#include "serializable.hpp"
#include <tbb/concurrent_unordered_map.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <sol.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <set>
#include <vector>
#include <utility>
#include <regex>

namespace BlueBear {
  namespace Graphics {
    class Shader;

    namespace SceneGraph {
      class Model;
      class Illuminator;

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
            Graphics::SceneGraph::ResourceBank cache;
            const ModelRegistration* previousMove = nullptr;
            std::unordered_map< std::string, std::shared_ptr< Graphics::SceneGraph::Model > > originals;
            std::unordered_map< std::string, std::shared_ptr< Graphics::SceneGraph::Illuminator > > illuminators;
            std::vector< std::unique_ptr< ModelRegistration > > models;
            Tools::AsyncTable asyncTasks;

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

          public:
            EXCEPTION_TYPE( ObjectIDNotRegisteredException, "Object ID not registered!" );
            WorldRenderer( Display& display );
            virtual ~WorldRenderer();

            Json::Value save() override;
            void load( const Json::Value& data ) override;

            void submitLuaContributions( sol::state& lua );

            void registerEvents( Device::Input::Input& inputManager );

            std::shared_ptr< Graphics::SceneGraph::Model > placeObject( const std::string& objectId, const std::set< std::string >& classes = {} );
            std::vector< std::shared_ptr< Graphics::SceneGraph::Model > > findObjectsByType( const std::string& instanceId );
            std::vector< std::shared_ptr< Graphics::SceneGraph::Model > > findObjectsByClass( const std::set< std::string >& classes );
            void removeObject( std::shared_ptr< Graphics::SceneGraph::Model > model );

            std::vector< std::shared_ptr< Graphics::SceneGraph::Illuminator > > findIlluminators( const std::regex& regex );
            std::shared_ptr< Graphics::SceneGraph::Illuminator > getIlluminator( const std::string& id );
            void addIlluminator( const std::string& id, std::shared_ptr< Graphics::SceneGraph::Illuminator > illuminator );
            void removeIlluminator( const std::string& id );

            unsigned int registerEvent(
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
            void onShaderChange();
            void nextFrame() override;
          };

        }
      }
    }
  }

}

#endif
