#include "display.hpp"
#include "configmanager.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>

namespace BlueBear {

  Display::Display() {
    // Get our settings out of the config manager
    x = ConfigManager::getInstance().getIntValue( "viewport_x" );
    y = ConfigManager::getInstance().getIntValue( "viewport_y" );
  }

  void Display::showDisplay() {
    // TODO: Localiser!
    mainWindow.create( sf::VideoMode( x, y ), "ne0ndrag0n BlueBear v0.0.3 \"Picasso\"", sf::Style::Close );
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
