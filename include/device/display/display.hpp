#ifndef DEVICE_DISPLAY
#define DEVICE_DISPLAY

#include <SFML/Graphics/RenderWindow.hpp>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <functional>

namespace BlueBear {
  namespace Device {
    namespace Display {

      namespace Adapter {
        class Adapter;
      }

      class Display {
        sf::RenderWindow window;
        const glm::uvec2 dimensions;
        std::vector< std::unique_ptr< Adapter::Adapter > > adapters;

      public:
        Display();
        ~Display();

        sf::RenderWindow& getRenderWindow();
        const glm::uvec2& getDimensions() const;
        Adapter::Adapter& pushAdapter( std::unique_ptr< Adapter::Adapter > adapter );
        Adapter::Adapter& getAdapterAt( unsigned int index );
        void executeOnSecondaryContext( std::function< void() > closure );
        void reset();
        void update();
      };

    }
  }
}

#endif
