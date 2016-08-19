#include "graphics/display.hpp"
#include "containers/collection3d.hpp"
#include "graphics/entity.hpp"
#include "scripting/lot.hpp"
#include "scripting/tile.hpp"
#include "threading/displaycommand.hpp"
#include "threading/enginecommand.hpp"
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

      displayCommandList = std::make_unique< Threading::Display::CommandList >();
    }

    void Display::render() {
      processIncomingCommands();

      // Handle events
      sf::Event event;
      while( mainWindow.pollEvent( event ) ) {
        if( event.type == sf::Event::Closed ) {
          mainWindow.close();
        }
      }

      mainWindow.clear( sf::Color::Black );
      mainWindow.display();

      processOutgoingCommands();
    }

    bool Display::isOpen() {
      return mainWindow.isOpen();
    }

    /**
     * Swap the pointers, and if the resulting list contains any commands, process those commands.
     */
    void Display::processIncomingCommands() {
      // The passed-in list should always be empty
      commandBus.attemptConsume( displayCommandList );

      for( auto& command : *displayCommandList ) {
        command->execute( *this );
      }

      displayCommandList->clear();
    }

    void Display::processOutgoingCommands() {
      if( engineCommandList.size() > 0 ) {
        commandBus.attemptProduce( engineCommandList );
      }
    }

    void Display::registerNewEntity() {
      engineCommandList.push_back( std::make_unique< Threading::Engine::RegisterInstance >( 42 ) );
    }

    /**
     * Given a lot, build instanceCollection and translate the Tiles/Wallpanels to instances on the lot
     */
    void Display::loadInfrastructure( Scripting::Lot& lot ) {
      instanceCollection = std::make_unique< Containers::Collection3D< std::shared_ptr< Instance > > >( lot.floorMap->levels, lot.floorMap->dimensionX, lot.floorMap->dimensionY );

      // Transform each Tile instance to an entity
      auto size = lot.floorMap->getLength();
      for( auto i = 0; i != size; i++ ) {
        auto tilePtr = lot.floorMap->getItemDirect( i );
        if( tilePtr ) {
          // There is a floor tile located here which needs to be drawn.
          auto tile = *tilePtr;
          
        } else {
          // There is no floor tile located here. Consequently, insert an empty Instance pointer here; it will be skipped on draw.
          instanceCollection->pushDirect( destPtr );
        }
      }
    }
  }
}
