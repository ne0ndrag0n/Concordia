#include "graphics/display.hpp"
#include "containers/collection3d.hpp"
#include "graphics/imagebuilder/imagesource.hpp"
#include "graphics/imagebuilder/pathimagesource.hpp"
#include "graphics/instance/instance.hpp"
#include "graphics/instance/wallinstance.hpp"
#include "graphics/instance/xwallinstance.hpp"
#include "graphics/instance/ywallinstance.hpp"
#include "graphics/instance/dwallinstance.hpp"
#include "graphics/instance/rwallinstance.hpp"
#include "graphics/shader.hpp"
#include "graphics/model.hpp"
#include "graphics/drawable.hpp"
#include "graphics/camera.hpp"
#include "graphics/material.hpp"
#include "graphics/texture.hpp"
#include "scripting/lot.hpp"
#include "scripting/tile.hpp"
#include "scripting/engine.hpp"
#include "scripting/wallcell.hpp"
#include "scripting/wallpaper.hpp"
#include "threading/commandbus.hpp"
#include "localemanager.hpp"
#include "configmanager.hpp"
#include "log.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <memory>
#include <mutex>
#include <string>
#include <cstdlib>
#include <utility>

static BlueBear::Graphics::Display::CommandList displayCommandList;
static BlueBear::Scripting::Engine::CommandList engineCommandList;

namespace BlueBear {
  namespace Graphics {

    const std::string Display::WALLPANEL_MODEL_XY_PATH = "system/models/wall/wall.dae";
    const std::string Display::WALLPANEL_MODEL_DR_PATH = "system/models/wall/diagwall.dae";
    const std::string Display::WALLATLAS_PATH = "system/models/wall/wallatlas.json";
    const std::string Display::FLOOR_MODEL_PATH = "system/models/floor/floor.dae";

    Display::Display( Threading::CommandBus& commandBus ) : commandBus( commandBus ) {
      // Get our settings out of the config manager
      x = ConfigManager::getInstance().getIntValue( "viewport_x" );
      y = ConfigManager::getInstance().getIntValue( "viewport_y" );

      // There must always be a defined State (avoid branch penalty/null check in tight loop)
      currentState = std::make_unique< IdleState >( *this );

      // Load defined fonts - Crash if we fail (game can't run without these fonts)
      if( !fonts.osdFont.loadFromFile( ConfigManager::getInstance().getValue( "font_osd" ) ) ) {
        Log::getInstance().error( "Display::Display", "Failed to load the OSD font!" );
        exit( 1 );
      }

      if( !fonts.uiFont.loadFromFile( ConfigManager::getInstance().getValue( "font_ui" ) ) ) {
        Log::getInstance().error( "Display::Display", "Failed to load the UI font!" );
        exit( 1 );
      }
    }

    Display::~Display() = default;

    void Display::openDisplay() {
      mainWindow.create( sf::VideoMode( x, y ), LocaleManager::getInstance().getString( "BLUEBEAR_WINDOW_TITLE" ), sf::Style::Close, sf::ContextSettings( 24, 8, 0, 3, 3 ) );

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
        currentState->execute();

        // Handle events
        sf::Event event;
        while( mainWindow.pollEvent( event ) ) {
          // This might be all we need for now
          switch( event.type ) {
            case sf::Event::Closed:
              mainWindow.close();
              break;
            default:
              // Any event not handled by Display itself
              // is passed along to currentState
              currentState->handleEvent( event );
              break;
          }
        }

        // Process outgoing commands
        if( engineCommandList.size() > 0 ) {
          commandBus.attemptProduce( engineCommandList );
        }
      }
    }

    /**
     * Given a lot, build floorInstanceCollection and translate the Tiles/Wallpanels to instances on the lot. Additionally, send the rotation status.
     */
    void Display::loadInfrastructure( Scripting::Lot& lot ) {

      std::unique_ptr< Display::MainGameState > mainGameStatePtr = std::make_unique< Display::MainGameState >( *this, lot );

      currentState = std::move( mainGameStatePtr );

      // Drop a SetLockState command for Engine
      engineCommandList.push_back( std::make_unique< Scripting::Engine::SetLockState >( true ) );
    }

    // ---------- STATES ----------

    Display::State::State( Display& instance ) : instance( instance ) {}

    // Does nothing. This is better than a null pointer check in a tight loop.
    Display::IdleState::IdleState( Display& instance ) : Display::State::State( instance ) {}
    void Display::IdleState::handleEvent( sf::Event& event ) {}
    void Display::IdleState::execute() {
      glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

      instance.mainWindow.display();
    }

    /**
     * Display renderer state for the titlescreen
     */
    Display::TitleState::TitleState( Display& instance ) : Display::State::State( instance ) {}
    void Display::TitleState::handleEvent( sf::Event& event ) {}
    void Display::TitleState::execute() {
      glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

      instance.mainWindow.display();
    }

    /**
     * Display renderer state for the main game loop
     */
    Display::MainGameState::MainGameState( Display& instance, Scripting::Lot& lot ) :
      Display::State::State( instance ),
      defaultShader( Shader( "system/shaders/default_vertex.glsl", "system/shaders/default_fragment.glsl" ) ),
      camera( Camera( defaultShader.Program, instance.x, instance.y ) ),
      floorModel( Model( Display::FLOOR_MODEL_PATH ) ) {

      // Load infrastructure models
      // Setup static pointers on each of the wall types
      auto xy = std::make_shared< Model >( Display::WALLPANEL_MODEL_XY_PATH );
      auto dr = std::make_shared< Model >( Display::WALLPANEL_MODEL_DR_PATH );
      XWallInstance::Piece = xy;
      YWallInstance::Piece = xy;
      DWallInstance::Piece = dr;
      RWallInstance::Piece = dr;

      // Setup camera
      camera.setRotationDirect( lot.currentRotation );

      texts.mode.setFont( instance.fonts.osdFont );
      texts.mode.setCharacterSize( 16 );
      texts.mode.setColor( sf::Color::White );
      texts.mode.setPosition( 0, 0 );

      texts.coords.setFont( instance.fonts.osdFont );
      texts.coords.setCharacterSize( 16 );
      texts.coords.setColor( sf::Color::Red );
      texts.coords.setPosition( 0, 16 );

      texts.direction.setFont( instance.fonts.osdFont );
      texts.direction.setCharacterSize( 16 );
      texts.direction.setColor( sf::Color::Green );
      texts.direction.setPosition( 0, 32 );

      texts.rotation.setFont( instance.fonts.osdFont );
      texts.rotation.setCharacterSize( 16 );
      texts.rotation.setColor( sf::Color::Blue );
      texts.rotation.setPosition( 0, 48 );

      // Moving much of Display::loadInfrastructure here
      loadInfrastructure( lot );
    }
    Display::MainGameState::~MainGameState() {
      XWallInstance::Piece.reset();
      YWallInstance::Piece.reset();

      DWallInstance::Piece.reset();
      RWallInstance::Piece.reset();
    }
    void Display::MainGameState::loadInfrastructure( Scripting::Lot& lot ) {
      floorInstanceCollection = std::make_unique< Containers::Collection3D< std::shared_ptr< Instance > > >( lot.floorMap->levels, lot.floorMap->dimensionX, lot.floorMap->dimensionY );
      wallInstanceCollection = std::make_unique< Containers::Collection3D< std::shared_ptr< Display::MainGameState::WallCellBundler > > >( lot.floorMap->levels, lot.floorMap->dimensionX, lot.floorMap->dimensionY );

      // Transform each Tile instance to an entity
      auto size = lot.floorMap->getLength();

      // TODO: Fix this unholy mess of counters and garbage
      int xOrigin = -( lot.floorMap->dimensionX / 2 );
      int yOrigin = lot.floorMap->dimensionY / 2;
      // Determines the floor level
      int tilesPerLevel = lot.floorMap->dimensionX * lot.floorMap->dimensionY;
      // The floor level is -10. Start this at -15 to avoid a branch penalty in the next if.
      float floorLevel = -15.0f;

      for( auto i = 0; i != size; i++ ) {

        int xCounter = xOrigin + ( i % lot.floorMap->dimensionX );
        int yCounter = yOrigin + -( i / lot.floorMap->dimensionY );

        // FLOOR
        // Increase level every time we exceed the number of tiles per level
        // Reset the boundaries of the 2D tile map
        if( i % tilesPerLevel == 0 ) {
          floorLevel = floorLevel + 5.0f;
        }

        auto tilePtr = lot.floorMap->getItemDirect( i );
        if( tilePtr ) {
          // Create instance from the model, and change its material using the material cache
          std::shared_ptr< Instance > instance = std::make_shared< Instance >( floorModel, defaultShader.Program );

          Drawable& floorDrawable = instance->drawables.at( "Plane" );

          floorDrawable.material = std::make_shared< Material >( TextureList{ texCache.get( tilePtr->imagePath ) } );

          instance->setPosition( glm::vec3( xCounter, yCounter, floorLevel ) );

          // The pointer to this floor tile goes into the floorInstanceCollection
          floorInstanceCollection->pushDirect( instance );
        } else {
          // There is no floor tile located here. Consequently, insert an empty Instance pointer here; it will be skipped on draw.
          floorInstanceCollection->pushDirect( std::shared_ptr< Instance >() );
        }

        // WALLS
        // Do not nudge any walls here (nudging will be the responsibility of MainGameState)
        // Nudging "X" means moving up in the Y dimension by 0.1
        // Nudging "Y" means moving left in the X dimension by 0.1
        auto wallCellPtr = lot.wallMap->getItemDirect( i );
        std::shared_ptr< Display::MainGameState::WallCellBundler > wallCellBundler;
        if( wallCellPtr ) {
          // Several different kinds of wall panel models depending on the type, and several kinds of orientations
          // If that pointer exists, at least one of these ifs will be fulfilled
          auto& bundler = getWallCellBundler( wallCellBundler );

          if( wallCellPtr->x ) {
            bundler.x = std::make_shared< XWallInstance >( defaultShader.Program, texCache, imageCache );
            bundler.x->setPosition( glm::vec3( xCounter, yCounter + 0.9f, floorLevel ) );
            bundler.x->setRotationAngle( glm::radians( 180.0f ) );

            bundler.x->setWallpaper( wallCellPtr->x->front->imagePath, wallCellPtr->x->back->imagePath );
            bundler.x->selectMaterial( lot.currentRotation );
          }

          if( wallCellPtr->y ) {
            bundler.y = std::make_shared< YWallInstance >( defaultShader.Program, texCache, imageCache );
            bundler.y->setRotationAngle( glm::radians( -90.0f ) );
            bundler.y->setPosition( glm::vec3( xCounter - 0.9f, yCounter, floorLevel ) );

            bundler.y->setWallpaper( wallCellPtr->y->front->imagePath, wallCellPtr->y->back->imagePath );
            bundler.y->selectMaterial( lot.currentRotation );
          }

          if( wallCellPtr->d ) {
            bundler.d = std::make_shared< DWallInstance >( defaultShader.Program, texCache, imageCache );
            bundler.d->setPosition( glm::vec3( xCounter, yCounter, floorLevel ) );

            bundler.d->setWallpaper( wallCellPtr->d->front->imagePath, wallCellPtr->d->back->imagePath );
            bundler.d->selectMaterial( lot.currentRotation );
          }

          if( wallCellPtr->r ) {
            bundler.r = std::make_shared< RWallInstance >( defaultShader.Program, texCache, imageCache );
            bundler.r->setRotationAngle( glm::radians( -90.0f ) );
            bundler.r->setPosition( glm::vec3( xCounter, yCounter, floorLevel ) );

            bundler.r->setWallpaper( wallCellPtr->r->front->imagePath, wallCellPtr->r->back->imagePath );
            bundler.r->selectMaterial( lot.currentRotation );
          }
        }

        wallInstanceCollection->pushDirect( wallCellBundler );
      }

      Log::getInstance().info( "Display::MainGameState::loadInfrastructure", "Finished creating infrastructure instances." );
    }
    Display::MainGameState::WallCellBundler& Display::MainGameState::getWallCellBundler( std::shared_ptr< Display::MainGameState::WallCellBundler >& bundlerPtr ) {
      if( !bundlerPtr ) {
        bundlerPtr = std::make_shared< Display::MainGameState::WallCellBundler >();
      }

      return *bundlerPtr;
    }
    void Display::MainGameState::execute() {
      glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

      // Use default shader and position camera
      defaultShader.use();
      camera.position();

      // Draw entities of each type
      // Floor & Walls with nudging
      auto length = floorInstanceCollection->getLength();
      for( auto i = 0; i != length; i++ ) {
        std::shared_ptr< Instance > floorInstance = floorInstanceCollection->getItemDirect( i );
        std::shared_ptr< Display::MainGameState::WallCellBundler > wallCellBundler = wallInstanceCollection->getItemDirect( i );
        auto rotation = camera.getCurrentRotation();
        float xWallNudge = ( rotation == 1 || rotation == 2 ) ? -0.1f : 0.0f;
        float yWallNudge = ( rotation == 0 || rotation == 1 ) ? 0.1f : 0.0f;

        if( floorInstance ) {
          floorInstance->drawEntity();
        }
        if( wallCellBundler ) {
          if( wallCellBundler->x ) {
            wallCellBundler->x->nudgeDrawEntity( glm::vec3( 0.0f, yWallNudge, 0.0f ) );
          }
          if( wallCellBundler->y ) {
            wallCellBundler->y->nudgeDrawEntity( glm::vec3( xWallNudge, 0.0f, 0.0f ) );
          }
        }
      }

      instance.mainWindow.pushGLStates();
        processOsd();
      instance.mainWindow.popGLStates();

      instance.mainWindow.display();
    }
    void Display::MainGameState::handleEvent( sf::Event& event ) {
      // Lay out some statics
      static int KEY_PERSPECTIVE = ConfigManager::getInstance().getIntValue( "key_switch_perspective" );
      static int KEY_ROTATE_RIGHT = ConfigManager::getInstance().getIntValue( "key_rotate_right" );
      static int KEY_ROTATE_LEFT = ConfigManager::getInstance().getIntValue( "key_rotate_left" );
      static int KEY_UP = ConfigManager::getInstance().getIntValue( "key_move_up" );
      static int KEY_DOWN = ConfigManager::getInstance().getIntValue( "key_move_down" );
      static int KEY_LEFT = ConfigManager::getInstance().getIntValue( "key_move_left" );
      static int KEY_RIGHT = ConfigManager::getInstance().getIntValue( "key_move_right" );

      // Main game has a few events mostly relating to camera
      switch( event.type ) {
        case sf::Event::KeyPressed:
          // absolutely disgusting
          {
            auto keyCode = event.key.code;

            if( keyCode == KEY_PERSPECTIVE ) {
              if( camera.ortho ) {
                camera.setOrthographic( false );
                //instance.mainWindow.setMouseCursorVisible( false );
                // there is no center yet
                //sf::Mouse::setPosition( center, instance.mainWindow );
                // just forget about this for now
                //instance.mainWindow.setFramerateLimit( 60 );
              } else {
                camera.setOrthographic( true );
                //instance.mainWindow.setMouseCursorVisible( true );
                // just forget about this for now
                //instance.mainWindow.setFramerateLimit( 30 );
              }
            }

            if( keyCode == KEY_ROTATE_RIGHT ) {
              camera.rotateRight();
              remapWallTextures();
            }

            if( keyCode == KEY_ROTATE_LEFT ) {
              camera.rotateLeft();
              remapWallTextures();
            }

            if( keyCode == KEY_UP ) {
              camera.move( 0.0f, 0.1f, 0.0f );
            }
            if( keyCode == KEY_DOWN ) {
              camera.move( 0.0f, -0.1f, 0.0f );
            }
            if( keyCode == KEY_LEFT ) {
              camera.move( -0.1f, 0.0f, 0.0f );
            }
            if( keyCode == KEY_RIGHT ) {
              camera.move( 0.1f, 0.0f, 0.0f );
            }

            break;
          }
        default:
          break;
      }
    }
    void Display::MainGameState::processOsd() {
      static std::string ISOMETRIC( LocaleManager::getInstance().getString( "ISOMETRIC" ) );
      static std::string FIRST_PERSON( LocaleManager::getInstance().getString( "FIRST_PERSON" ) );
      static std::string ROTATION( LocaleManager::getInstance().getString( "ROTATION" ) );

      texts.mode.setString( camera.ortho ? ISOMETRIC : FIRST_PERSON );
      texts.coords.setString( camera.positionToString().c_str() );
      texts.direction.setString( camera.directionToString().c_str() );
      texts.rotation.setString( ROTATION + ": " + std::to_string( camera.getCurrentRotation() ) );

      instance.mainWindow.draw( texts.mode );
      instance.mainWindow.draw( texts.coords );
      instance.mainWindow.draw( texts.direction );
      instance.mainWindow.draw( texts.rotation );
    }
    void Display::MainGameState::remapWallTextures() {
      unsigned int currentRotation = camera.getCurrentRotation();
      unsigned int mapSize = wallInstanceCollection->getLength();

      for( unsigned int i = 0; i != mapSize; i++ ) {
        auto ptr = wallInstanceCollection->getItemDirect( i );
        if( ptr ) {
          auto& wallCellBundler = *ptr;

          if( wallCellBundler.x ) {
            wallCellBundler.x->selectMaterial( currentRotation );
          }
          if( wallCellBundler.y ) {
            wallCellBundler.y->selectMaterial( currentRotation );
          }
          if( wallCellBundler.d ) {
            wallCellBundler.d->selectMaterial( currentRotation );
          }
          if( wallCellBundler.r ) {
            wallCellBundler.r->selectMaterial( currentRotation );
          }
        }
      }
    }

    // ---------- COMMANDS ----------
    void Display::NewEntityCommand::execute( Graphics::Display& instance ) {
      Log::getInstance().info( "NewEntityCommand", "Called registerNewEntity, hang in there..." );
    }

    Display::SendInfrastructureCommand::SendInfrastructureCommand( Scripting::Lot& lot ) : lot( lot ) {}
    void Display::SendInfrastructureCommand::execute( Graphics::Display& instance ) {
      instance.loadInfrastructure( lot );
    }
  }
}
