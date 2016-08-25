#ifndef BBSCREEN
#define BBSCREEN

/**
 * Abstracted class representing the display device that is available to BlueBear.
 */
#include "containers/collection3d.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <list>
#include <glm/glm.hpp>
#include "graphics/entity.hpp"
#include "graphics/materialcache.hpp"
#include "graphics/model.hpp"
#include "graphics/shader.hpp"

namespace BlueBear {
  namespace Scripting {
    class Lot;
  }

  namespace Threading {
    class CommandBus;
  }

  namespace Graphics {
    class Instance;

    class Display {

      // ---------- STATES ----------
      class State {
        public:
          virtual void execute() = 0;
      };

      class IdleState : public State {
        void execute();
      };

      class TitleState : public State {
        void execute();
      };

      class MainGameState : public State {
        void execute();
      };
      // ----------------------------

    public:
      // RAII style
      Display( Threading::CommandBus& commandBus );

      void openDisplay();
      void render();
      bool isOpen();
      void loadInfrastructure( Scripting::Lot& lot );

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

      using CommandList = std::list< std::unique_ptr< Command > >;

      private:
        static constexpr const char* FLOOR_MODEL_PATH = "system/models/floor/floor.dae";
        using ViewportDimension = int;
        ViewportDimension x;
        ViewportDimension y;
        std::vector< Instance > instances;
        sf::RenderWindow mainWindow;
        Threading::CommandBus& commandBus;

        std::unique_ptr< Containers::Collection3D< std::shared_ptr< Instance > > > instanceCollection;

        std::unique_ptr< Model > floorModel;
        std::unique_ptr< Model > wallPanelModel;

        std::unique_ptr< Shader > defaultShader;

        std::unique_ptr< State > currentState;

        MaterialCache materialCache;

        void main();
    };

  }
}

#endif
