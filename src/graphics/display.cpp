#include "graphics/display.hpp"
#include "containers/collection3d.hpp"
#include "graphics/entity.hpp"
#include "graphics/shader.hpp"
#include "graphics/model.hpp"
#include "graphics/drawable.hpp"
#include "scripting/lot.hpp"
#include "scripting/tile.hpp"
#include "scripting/engine.hpp"
#include "threading/commandbus.hpp"
#include "localemanager.hpp"
#include "configmanager.hpp"
#include "log.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <GL/glew.h>
#include <memory>
#include <mutex>
#include <string>
#include <cstdlib>

static BlueBear::Graphics::Display::CommandList displayCommandList;
static BlueBear::Scripting::Engine::CommandList engineCommandList;

namespace BlueBear {
  namespace Graphics {

    Display::Display( Threading::CommandBus& commandBus ) : commandBus( commandBus ) {
      // Get our settings out of the config manager
      x = ConfigManager::getInstance().getIntValue( "viewport_x" );
      y = ConfigManager::getInstance().getIntValue( "viewport_y" );

      // There must always be a defined State (avoid branch penalty/null check in tight loop)
      currentState = std::make_unique< IdleState >( *this );
    }

    Display::~Display() = default;

    void Display::openDisplay() {
      mainWindow.create( sf::VideoMode( x, y ), LocaleManager::getInstance().getString( "BLUEBEAR_WINDOW_TITLE" ), sf::Style::Close );

      // Set sync on window by these params:
      // vsync_limiter_overview = true or fps_overview
      if( ConfigManager::getInstance().getBoolValue( "vsync_limiter_overview" ) == true ) {
        mainWindow.setVerticalSyncEnabled( true );
      } else {
        mainWindow.setFramerateLimit( ConfigManager::getInstance().getIntValue( "fps_overview" ) );
      }

      // Initialize OpenGL using GLEW
      glewExperimental = true;
      auto glewStatus = glewInit();
      if( glewStatus != GLEW_OK ) {
        // Oh, this piece of shit function. Why the fuck is this GLubyte* when every other god damn string type is const char*?!
        Log::getInstance().error( "Display::openDisplay", "FATAL: glewInit() did NOT return GLEW_OK! (" + std::string( ( const char* ) glewGetErrorString( glewStatus ) ) + ")" );
        exit( 1 );
      }

      // Open default shaders
      // FIXME: constexpr in g++ is totally, utterly broken and i do not want to use macro constants
      // because C-style idioms are banned from this project
      Log::getInstance().debug( "Display::openDisplay", "Loading the default shader..." );
      defaultShader = std::make_unique< Shader >( "system/shaders/default_vertex.glsl", "system/shaders/default_fragment.glsl" );
      Log::getInstance().debug( "Display::openDisplay", "Done" );

      // There may be more than just this needed from main.cpp in the area51/sfml_test project
      glViewport( 0, 0, x, y );
      glEnable( GL_DEPTH_TEST );
      glEnable( GL_CULL_FACE );
    }

    void Display::start() {
      while( mainWindow.isOpen() ) {
        // Process incoming commands - the passed-in list should always be empty
        commandBus.attemptConsume( displayCommandList );

        for( auto& command : displayCommandList ) {
          command->execute( *this );
        }

        displayCommandList.clear();

        // Handle rendering
        mainWindow.clear( sf::Color::Black );
        currentState->execute();
        mainWindow.display();

        // Handle events
        sf::Event event;
        while( mainWindow.pollEvent( event ) ) {
          if( event.type == sf::Event::Closed ) {
            mainWindow.close();
          }
        }

        // Process outgoing commands
        if( engineCommandList.size() > 0 ) {
          commandBus.attemptProduce( engineCommandList );
        }
      }
    }

    /**
     * Given a lot, build instanceCollection and translate the Tiles/Wallpanels to instances on the lot
     */
    void Display::loadInfrastructure( Scripting::Lot& lot ) {
      instanceCollection = std::make_unique< Containers::Collection3D< std::shared_ptr< Instance > > >( lot.floorMap->levels, lot.floorMap->dimensionX, lot.floorMap->dimensionY );

      // Lazy-load floorPanel and wallPanelModel
      if( !floorModel ) {
        // wrapped in std::string - compiler bug causes the constexpr not to be evaluated, and fails in linking
        floorModel = std::make_unique< Model >( std::string( FLOOR_MODEL_PATH ) );
      }

      // Transform each Tile instance to an entity
      auto size = lot.floorMap->getLength();
      for( auto i = 0; i != size; i++ ) {
        auto tilePtr = lot.floorMap->getItemDirect( i );
        if( tilePtr ) {
          // Create instance from the model, and change its material using the material cache
          std::shared_ptr< Instance > instance = std::make_shared< Instance >( *floorModel, defaultShader->Program );
          Drawable& floorDrawable = instance->drawables.at( "Plane" );
          floorDrawable.material = materialCache.get( *tilePtr );

          // The pointer to this floor tile goes into the instanceCollection
          instanceCollection->pushDirect( instance );
        } else {
          // There is no floor tile located here. Consequently, insert an empty Instance pointer here; it will be skipped on draw.
          instanceCollection->pushDirect( std::shared_ptr< Instance >() );
        }
      }

      Log::getInstance().info( "Display::loadInfrastructure", "Finished creating infrastructure instances." );

      // Drop a SetLockState command for Engine
      engineCommandList.push_back( std::make_unique< Scripting::Engine::SetLockState >( true ) );
    }

    // ---------- STATES ----------

    Display::State::State( Display& instance ) : instance( instance ) {}

    // Does nothing. This is better than a null pointer check in a tight loop.
    Display::IdleState::IdleState( Display& instance ) : Display::State::State( instance ) {}
    void Display::IdleState::execute() {}

    /**
     * Display renderer state for the titlescreen
     */
    Display::TitleState::TitleState( Display& instance ) : Display::State::State( instance ) {}
    void Display::TitleState::execute() {

    }

    /**
     * Display renderer state for the main game loop
     */
    Display::MainGameState::MainGameState( Display& instance ) : Display::State::State( instance ) {

    }
    Display::MainGameState::~MainGameState() {

    }
    void Display::MainGameState::execute() {

    }

    // ---------- COMMANDS ----------
    void Display::NewEntityCommand::execute( Graphics::Display& instance ) {
      Log::getInstance().info( "NewEntityCommand", "Called registerNewEntity, hang in there..." );
    }

    Display::SendInfrastructureCommand::SendInfrastructureCommand( Scripting::Lot& lot ) : lot( lot ) {}
    void Display::SendInfrastructureCommand::execute( Graphics::Display& instance ) {
      instance.loadInfrastructure( lot );
    }

    Display::ChangeStateCommand::ChangeStateCommand( Display::ChangeStateCommand::State selectedState ) : selectedState( selectedState ) {}
    void Display::ChangeStateCommand::execute( Graphics::Display& instance ) {
      switch( selectedState ) {
        case State::STATE_TITLESCREEN:
          instance.currentState = std::make_unique< Display::TitleState >( instance );
          break;
        case State::STATE_MAINGAME:
          instance.currentState = std::make_unique< Display::MainGameState >( instance );
          break;
        case State::STATE_IDLE:
        default:
          instance.currentState = std::make_unique< Display::IdleState >( instance );
      }
    }
  }
}
