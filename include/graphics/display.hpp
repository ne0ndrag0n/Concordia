#ifndef BBSCREEN
#define BBSCREEN

/**
 * Abstracted class representing the display device that is available to BlueBear.
 */
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
          struct {
            std::string ISOMETRIC;
            std::string FIRST_PERSON;
          } strings;
          unsigned int currentRotation;
          Shader defaultShader;
          Camera camera;
          Model floorModel;
          TextureCache texCache;
          ImageCache imageCache;
          struct {
            sfg::Desktop desktop;
            std::shared_ptr< sfg::Container > rootContainer;
            sf::Clock clock;
          } gui;
          // These are from the lot!
          Containers::Collection3D< std::shared_ptr< Scripting::Tile > >& floorMap;
          Containers::Collection3D< std::shared_ptr< Scripting::WallCell > >& wallMap;
          // These are ours!
          std::unique_ptr< Containers::Collection3D< std::shared_ptr< Instance > > > floorInstanceCollection;
          std::unique_ptr< Containers::Collection3D< std::shared_ptr< WallCellBundler > > > wallInstanceCollection;
          void processOsd();
          void loadInfrastructure();
          void createFloorInstances();
          void createWallInstances();
          void setupGUI();
          void submitLuaContributions();
          sfg::Desktop getDesktopFromXML();
          static int lua_loadXMLWidgets( lua_State* L );
          static int lua_Widget_gc( lua_State* L );
          static int lua_Widget_getWidgetByID( lua_State* L );
          static int lua_Widget_onEvent( lua_State* L );
          static int lua_getWidgetByID( lua_State* L );
        public:
          void execute();
          void handleEvent( sf::Event& event );
          MainGameState( Display& instance, unsigned int currentRotation, Containers::Collection3D< std::shared_ptr< Scripting::Tile > >& floorMap, Containers::Collection3D< std::shared_ptr< Scripting::WallCell > >& wallMap );
          ~MainGameState();
      };
      // ----------------------------

      private:
        lua_State* L;
        EventManager& eventManager;
        using ViewportDimension = int;
        ViewportDimension x;
        ViewportDimension y;
        sf::RenderWindow mainWindow;
        sfg::SFGUI sfgui;

        std::unique_ptr< State > currentState;

        friend class Display::MainGameState;
    };

  }
}

#endif
