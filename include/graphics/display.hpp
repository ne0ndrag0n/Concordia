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

      class Command {
        public:
          virtual void execute( Display& instance ) = 0;
      };

      class NewEntityCommand : public Command {
        glm::vec3 position;
        public:
          void execute( Display& instance );
      };

      using CommandList = std::vector< Command >;

      private:
        using ViewportDimension = int;
        ViewportDimension x;
        ViewportDimension y;
        std::vector< Instance > instances;
        sf::RenderWindow mainWindow;
        Threading::CommandBus& commandBus;
        std::unique_ptr< CommandList > displayCommands;
        void main();
        void processCommands();
    };

  }
}

#endif
