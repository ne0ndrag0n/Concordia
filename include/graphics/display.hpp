#ifndef BBSCREEN
#define BBSCREEN

/**
 * Abstracted class representing the display device that is available to BlueBear.
 */
#include <SFML/Graphics.hpp>

namespace BlueBear {
  namespace Graphics {

    class Display {

      using ViewportDimension = int;

      private:
        ViewportDimension x;
        ViewportDimension y;
        sf::RenderWindow mainWindow;
        void main();

      public:
        // RAII style
        Display();

        void showDisplay();
    };

  }
}

#endif
