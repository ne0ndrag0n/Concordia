#ifndef BBSCREEN
#define BBSCREEN

/**
 * Abstracted class representing the display device that is available to BlueBear.
 */
#include "bbtypes.hpp"
#include "containers/collection3d.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Widgets.hpp>
#include <vector>
#include <memory>
#include <list>
#include <glm/glm.hpp>
#include <string>
#include <map>
#include "graphics/texturecache.hpp"
#include "graphics/imagecache.hpp"
#include "scripting/tile.hpp"
#include "scripting/wallcell.hpp"
#include "graphics/gui/sfgroot.hpp"
#include "graphics/imagebuilder/imagesource.hpp"
#include "graphics/shader.hpp"
#include "graphics/model.hpp"
#include "graphics/camera.hpp"
#include "graphics/instance/instance.hpp"
#include "graphics/input/inputmanager.hpp"

namespace BlueBear {
  class EventManager;

  namespace Scripting {
    class Lot;
  }

  namespace Threading {
    class CommandBus;
  }

  namespace Graphics {
    class Instance;
    class WallInstance;
    class XWallInstance;
    class YWallInstance;
    class DWallInstance;
    class RWallInstance;
    class WallCellBundler;
    class ShaderInstanceBundle;

    class Display {
    public:
      static const std::string WALLPANEL_MODEL_XY_PATH;
      static const std::string WALLPANEL_MODEL_DR_PATH;
      static const std::string FLOOR_MODEL_PATH;

      // RAII style
      Display( lua_State* L, EventManager& eventManager );
      ~Display();

      void openDisplay();
      bool update();
      void changeToMainGameState( unsigned int currentRotation, Containers::Collection3D< std::shared_ptr< Scripting::Tile > >& floorMap, Containers::Collection3D< std::shared_ptr< Scripting::WallCell > >& wallMap );

      // ---------- STATES ----------
      class State {
        public:
          Display& instance;
          State( Display& instance );
          virtual ~State() = default;
          virtual void execute() = 0;
          virtual void handleEvent( sf::Event& event ) = 0;
      };

      class IdleState : public State {
        public:
          IdleState( Display& instance );
          void execute();
          void handleEvent( sf::Event& event );
      };

      class TitleState : public State {
        public:
          TitleState( Display& instance );
          void execute();
          void handleEvent( sf::Event& event );
      };

      class MainGameState : public State {
          lua_State* L;
          Input::InputManager inputManager;
          unsigned int currentRotation;
          Shader defaultShader;
          Camera camera;
          std::unique_ptr< Model > floorModel;
          std::unique_ptr< Model > __debugModel;
          TextureCache texCache;
          ImageCache imageCache;
          std::vector< ShaderInstanceBundle > dynamicInstances;
          // These are from the lot!
          Containers::Collection3D< std::shared_ptr< Scripting::Tile > >& floorMap;
          Containers::Collection3D< std::shared_ptr< Scripting::WallCell > >& wallMap;
          // These are ours!
          std::unique_ptr< Containers::Collection3D< std::shared_ptr< Instance > > > floorInstanceCollection;
          std::unique_ptr< Containers::Collection3D< std::shared_ptr< WallCellBundler > > > wallInstanceCollection;
          void registerEvents();
          void loadIntrinsicModels();
          void processOsd();
          void loadInfrastructure();
          void createFloorInstances();
          void createWallInstances();
          void setupGUI();
          void submitLuaContributions();
          static int lua_rotateWorldLeft( lua_State* L );
          static int lua_rotateWorldRight( lua_State* L );
          static int lua_zoomIn( lua_State* L );
          static int lua_zoomOut( lua_State* L );
          static int lua_loadXMLWidgets( lua_State* L );

          // XXX: remove after demo branch
          static int lua_playanim1( lua_State* L );
          static int lua_playanim2( lua_State* L );
        public:
          struct {
            sfg::Desktop desktop;
            std::shared_ptr< sfg::Container > rootContainer;
            sf::Clock clock;
          } gui;

          void execute();
          void handleEvent( sf::Event& event );
          MainGameState( Display& instance, unsigned int currentRotation, Containers::Collection3D< std::shared_ptr< Scripting::Tile > >& floorMap, Containers::Collection3D< std::shared_ptr< Scripting::WallCell > >& wallMap );
          ~MainGameState();
      };
      // ----------------------------

        EventManager& eventManager;
      private:
        lua_State* L;
        using ViewportDimension = int;
        ViewportDimension x;
        ViewportDimension y;
        sf::RenderWindow mainWindow;
        sfg::SFGUI sfgui;

        std::unique_ptr< State > currentState;
    };

  }
}

#endif
