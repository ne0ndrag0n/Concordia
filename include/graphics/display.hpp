#ifndef BBSCREEN
#define BBSCREEN

/**
 * Abstracted class representing the display device that is available to BlueBear.
 */
#include "containers/collection3d.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <memory>
#include <list>
#include <glm/glm.hpp>
#include "graphics/materialcache.hpp"
// Compiler won't accept an incomplete type for Camera. It accepts EVERY other one.
// You can be the one to fuckin' tell me why.
#include "graphics/camera.hpp"

namespace BlueBear {
  namespace Scripting {
    class Lot;
  }

  namespace Threading {
    class CommandBus;
  }

  namespace Graphics {
    class Instance;
    class Model;
    class Shader;
    //class Camera;

    class Display {

    public:
      // RAII style
      Display( Threading::CommandBus& commandBus );
      ~Display();

      void openDisplay();
      void start();
      void loadInfrastructure( Scripting::Lot& lot );

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
          struct {
            sf::Text mode;
            sf::Text coords;
            sf::Text direction;
            sf::Text rotation;
          } texts;
          struct {
            std::string ISOMETRIC;
            std::string FIRST_PERSON;
          } strings;
          void processOsd();
        public:
          void execute();
          void handleEvent( sf::Event& event );
          MainGameState( Display& instance );
          ~MainGameState();
      };
      // ----------------------------

      class Command {
        public:
          virtual void execute( Display& instance ) = 0;
      };

      class NewEntityCommand : public Command {
        glm::vec3 position;
        public:
          void execute( Display& instance );
      };

      class SendInfrastructureCommand : public Command {
        Scripting::Lot& lot;
        public:
          SendInfrastructureCommand( Scripting::Lot& lot );
          void execute( Display& instance );
      };

      class ChangeStateCommand : public Command {
        public:
          enum class State { STATE_IDLE, STATE_TITLESCREEN, STATE_MAINGAME };

          ChangeStateCommand( State selectedState );
          void execute( Display& instance );

        private:
          State selectedState;
      };

      using CommandList = std::list< std::unique_ptr< Command > >;

      private:
        static constexpr const char* FLOOR_MODEL_PATH = "system/models/floor/floor.dae";
        using ViewportDimension = int;
        // Wrapper class that holds four instances per tile
        struct WallCellBundler {
          std::shared_ptr< Instance > x;
          std::shared_ptr< Instance > y;
          std::shared_ptr< Instance > d;
          std::shared_ptr< Instance > r;
        };
        ViewportDimension x;
        ViewportDimension y;
        std::vector< Instance > instances;
        sf::RenderWindow mainWindow;
        Threading::CommandBus& commandBus;

        std::unique_ptr< Containers::Collection3D< std::shared_ptr< Instance > > > floorInstanceCollection;
        std::unique_ptr< Containers::Collection3D< WallCellBundler > > wallInstanceCollection;

        std::unique_ptr< Model > floorModel;
        std::unique_ptr< Model > wallPanelModel;

        std::unique_ptr< Camera > camera;
        std::unique_ptr< Shader > defaultShader;

        std::unique_ptr< State > currentState;

        struct {
          sf::Font osdFont;
          sf::Font uiFont;
        } fonts;

        // This should last the life of the Display instance.
        MaterialCache materialCache;

        void main();
    };

  }
}

#endif
