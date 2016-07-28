#ifndef BBSCREEN
#define BBSCREEN

/**
 * Abstracted class representing the display device that is available to BlueBear.
 */
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "graphics/entity.hpp"
#include "threading/displaycommand.hpp"
#include "threading/enginecommand.hpp"

namespace BlueBear {
  namespace Threading {
    class CommandBus;
  }

  namespace Graphics {

    class Display {

    public:
      // RAII style
      Display( Threading::CommandBus& commandBus );

      void openDisplay();
      void render();
      bool isOpen();
      void registerNewEntity();

      private:
        using ViewportDimension = int;
        ViewportDimension x;
        ViewportDimension y;
        std::vector< Instance > instances;
        sf::RenderWindow mainWindow;
        Threading::CommandBus& commandBus;
        std::unique_ptr< Threading::Display::CommandList > displayCommandList;
        Threading::Engine::CommandList engineCommandList;
        void main();
        void processIncomingCommands();
        void processOutgoingCommands();
    };

  }
}

#endif
