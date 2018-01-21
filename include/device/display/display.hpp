#ifndef DEVICE_DISPLAY
#define DEVICE_DISPLAY

#include <SFML/Graphics/RenderWindow.hpp>
#include <glm/glm.hpp>

namespace BlueBear {
  namespace Device {
    namespace Display {

      namespace Adapter {
        class Adapter;
      }

      class Display {
        sf::RenderWindow window;
        const glm::uvec2 dimensions;
        std::unique_ptr< Adapter::Adapter > adapter;

      public:
        Display();
        ~Display();

        sf::RenderWindow& getRenderWindow();
        const glm::uvec2& getDimensions() const;
        void setAdapter( std::unique_ptr< Adapter::Adapter >& adapter );
        void update();
      };

    }
  }
}

#endif
