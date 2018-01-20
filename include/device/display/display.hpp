#ifndef DEVICE_DISPLAY
#define DEVICE_DISPLAY

#include <SFML/Graphics/RenderWindow.hpp>

namespace BlueBear {
  namespace Device {
    namespace Display {

      namespace Adapter {
        class Adapter;
      }

      class Display {
        sf::RenderWindow window;
        const unsigned int x;
        const unsigned int y;
        std::unique_ptr< Adapter::Adapter > adapter;

      public:
        Display();
        ~Display();

        sf::RenderWindow& getRenderWindow();
        void setAdapter( std::unique_ptr< Adapter::Adapter >& adapter );
        void update();
      };

    }
  }
}

#endif
