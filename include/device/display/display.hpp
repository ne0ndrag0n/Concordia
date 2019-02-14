#ifndef DEVICE_DISPLAY
#define DEVICE_DISPLAY

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/ContextSettings.hpp>
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
        // Device::Display::Display doesn't own the adapters!!
        // These objects are owned by the associated state objects
        std::vector< Adapter::Adapter* > adapters;

        void printWelcomeMessage();

      public:
        Display();
        ~Display();

        sf::ContextSettings getDefaultContextSettings() const;
        sf::RenderWindow& getRenderWindow();
        const glm::uvec2& getDimensions() const;
        Adapter::Adapter& pushAdapter( Adapter::Adapter* adapter );
        Adapter::Adapter& getAdapterAt( unsigned int index );
        void executeOnSecondaryContext( std::function< void() > closure );
        void executeOnSecondaryContext( sf::Context& context, std::function< void() > closure );
        void reset();
        void update();
      };

    }
  }
}

#endif
