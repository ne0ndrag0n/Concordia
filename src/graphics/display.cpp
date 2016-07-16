#include "graphics/display.hpp"
#include "localemanager.hpp"
#include "configmanager.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>

namespace BlueBear {
  namespace Graphics {
    Display::Display() {
      // Get our settings out of the config manager
      x = ConfigManager::getInstance().getIntValue( "viewport_x" );
      y = ConfigManager::getInstance().getIntValue( "viewport_y" );
    }

    void Display::showDisplay() {
      mainWindow.create( sf::VideoMode( x, y ), LocaleManager::getInstance().getString( "BLUEBEAR_WINDOW_TITLE" ), sf::Style::Close );
      mainWindow.setVerticalSyncEnabled( true );
      main();
    }

    void Display::main() {
      while( mainWindow.isOpen() ) {
        // Handle events
        sf::Event event;
        while( mainWindow.pollEvent( event ) ) {
          if( event.type == sf::Event::Closed ) {
            mainWindow.close();
          }
        }

        mainWindow.clear( sf::Color::Black );
        mainWindow.display();
      }
    }
  }
}
