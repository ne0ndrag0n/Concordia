#ifndef BBSCREEN
#define BBSCREEN

/**
 * Abstracted class representing the display device that is available to BlueBear.
 */
#include "containers/collection3d.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "graphics/entity.hpp"
#include "threading/displaycommand.hpp"
#include "threading/enginecommand.hpp"
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

    public:
      // RAII style
      Display( Threading::CommandBus& commandBus );

      void openDisplay();
      void render();
      bool isOpen();
      void registerNewEntity();
      void loadInfrastructure( Scripting::Lot& lot );

      private:
        static constexpr const char* FLOOR_MODEL_PATH = "system/models/floor/floor.dae";
        using ViewportDimension = int;
        ViewportDimension x;
        ViewportDimension y;
        std::vector< Instance > instances;
        sf::RenderWindow mainWindow;
        Threading::CommandBus& commandBus;
        std::unique_ptr< Threading::Display::CommandList > displayCommandList;
        std::unique_ptr< Containers::Collection3D< std::shared_ptr< Instance > > > instanceCollection;
        Threading::Engine::CommandList engineCommandList;

        std::unique_ptr< Model > floorModel;
        std::unique_ptr< Model > wallPanelModel;

        std::unique_ptr< Shader > defaultShader;

        void main();
        void processIncomingCommands();
        void processOutgoingCommands();
    };

  }
}

#endif
