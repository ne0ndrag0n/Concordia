#ifndef WORLD_RENDERER
#define WORLD_RENDERER

#include "device/display/adapter/adapter.hpp"
#include "graphics/camera.hpp"
#include "graphics/scenegraph/resourcebank.hpp"
#include "exceptions/genexc.hpp"
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

      namespace Light {
        class Light;
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
            struct ModelRegistration {
              const std::string originalId;
              const std::set< std::string > instanceClasses;
              std::shared_ptr< Graphics::SceneGraph::Model > instance;

              bool operator<( const ModelRegistration& rhs ) const {
                return instance.get() < rhs.instance.get();
              };
            };

            Graphics::Camera camera;
            Graphics::SceneGraph::ResourceBank cache;
            std::unordered_map< std::string, std::shared_ptr< Graphics::SceneGraph::Model > > originals;
            std::unordered_map< std::string, std::shared_ptr< Graphics::SceneGraph::Light::Light > > lights;
            std::set< ModelRegistration > models;

            std::unique_ptr< Graphics::SceneGraph::ModelLoader::FileModelLoader > getFileModelLoader( bool deferGLOperations );

          public:
            EXCEPTION_TYPE( ObjectIDNotRegisteredException, "Object ID not registered!" );
            WorldRenderer( Display& display );
            virtual ~WorldRenderer();

            Json::Value save() override;
            void load( const Json::Value& data ) override;

            void submitLuaContributions( sol::state& lua );

            std::shared_ptr< Graphics::SceneGraph::Model > placeObject( const std::string& objectId, const std::set< std::string >& classes = {} );
            std::vector< std::shared_ptr< Graphics::SceneGraph::Model > > findObjectsByType( const std::string& instanceId );
            std::vector< std::shared_ptr< Graphics::SceneGraph::Model > > findObjectsByClass( const std::set< std::string >& classes );
            void removeObject( std::shared_ptr< Graphics::SceneGraph::Model > model );

            std::vector< std::shared_ptr< Graphics::SceneGraph::Light::Light > > findLightsByRegex( const std::regex& regex );

            Graphics::Camera& getCamera();
            void loadPathsParallel( const std::vector< std::pair< std::string, std::string > >& paths );
            void loadPaths( const std::vector< std::pair< std::string, std::string > >& paths );
            void insertDirect( const std::shared_ptr< Graphics::SceneGraph::Model >& model );
            void onShaderChange();
            void nextFrame() override;
          };

        }
      }
    }
  }

}

#endif
