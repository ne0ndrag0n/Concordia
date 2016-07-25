#include "graphics/display.hpp"
#include "threading/displaycommand.hpp"
#include "threading/commandbus.hpp"
#include "localemanager.hpp"
#include "configmanager.hpp"
#include "log.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <memory>
#include <mutex>

namespace BlueBear {
  namespace Graphics {

    Display::Display( Threading::CommandBus& commandBus ) : commandBus( commandBus ) {
      // Get our settings out of the config manager
      x = ConfigManager::getInstance().getIntValue( "viewport_x" );
      y = ConfigManager::getInstance().getIntValue( "viewport_y" );
    }

    void Display::openDisplay() {
      mainWindow.create( sf::VideoMode( x, y ), LocaleManager::getInstance().getString( "BLUEBEAR_WINDOW_TITLE" ), sf::Style::Close );
      mainWindow.setVerticalSyncEnabled( true );
    }

    void Display::render() {
      processCommands();

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

    bool Display::isOpen() {
      return mainWindow.isOpen();
    }

    /**
     * Swap the pointers, and if the resulting list contains any commands, process those commands.
     */
    void Display::processCommands() {
      std::unique_ptr< Threading::Display::CommandSeries > localCommandSeries = std::make_unique< Threading::Display::CommandSeries >();
      commandBus.consume( localCommandSeries );

      for( auto& commandList : *localCommandSeries ) {
        for( auto& command : *commandList ) {
          command->execute( *this );
        }
      }

      // When this function concludes, the old commandlist and its accompanying commands will fall out of scope and be deallocated
    }
  }
}
