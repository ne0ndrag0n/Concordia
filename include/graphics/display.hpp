#ifndef BBSCREEN
#define BBSCREEN

/**
 * Abstracted class representing the display device that is available to BlueBear.
 */
#include <SFML/Graphics.hpp>
#include <vector>
#include "graphics/entity.hpp"

namespace BlueBear {
  namespace Graphics {

    class Display {

      using ViewportDimension = int;

      private:
        ViewportDimension x;
        ViewportDimension y;
        std::vector< Instance > instances;
        sf::RenderWindow mainWindow;
        void main();

      public:
        // RAII style
        Display();

        void openDisplay();
        void render();
        bool isOpen();
    };

  }
}

#endif
