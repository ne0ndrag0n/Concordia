#include "graphics/display.hpp"
#include "containers/collection3d.hpp"
#include "containers/conccollection3d.hpp"
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
#include "threading/lockable.hpp"
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
#include <functional>

static BlueBear::Graphics::Display::CommandList displayCommandList;
static BlueBear::Scripting::Engine::CommandList engineCommandList;

namespace BlueBear {
  namespace Graphics {

    const std::string Display::WALLPANEL_MODEL_XY_PATH = "system/models/wall/wall.dae";
    const std::string Display::WALLPANEL_MODEL_XY_EDGE_PATH = "system/models/wall/wall_large.dae";
    const std::string Display::WALLPANEL_MODEL_DR_PATH = "system/models/wall/diagwall.dae";
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
    void Display::loadInfrastructure( unsigned int currentRotation, Containers::ConcCollection3D< Threading::Lockable< Scripting::Tile > >& floorMap, Containers::ConcCollection3D< Threading::Lockable< Scripting::WallCell > >& wallMap ) {

      std::unique_ptr< Display::MainGameState > mainGameStatePtr = std::make_unique< Display::MainGameState >( *this, currentRotation, floorMap, wallMap );

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
    Display::MainGameState::MainGameState( Display& instance, unsigned int currentRotation, Containers::ConcCollection3D< Threading::Lockable< Scripting::Tile > >& floorMap, Containers::ConcCollection3D< Threading::Lockable< Scripting::WallCell > >& wallMap ) :
      Display::State::State( instance ),
      defaultShader( Shader( "system/shaders/default_vertex.glsl", "system/shaders/default_fragment.glsl" ) ),
      camera( Camera( defaultShader.Program, instance.x, instance.y ) ),
      floorModel( Model( Display::FLOOR_MODEL_PATH ) ),
      floorMap( floorMap ),
      wallMap( wallMap ),
      currentRotation( currentRotation ) {

      // Load infrastructure models
      // Setup static pointers on each of the wall types
      auto xy = std::make_shared< Model >( Display::WALLPANEL_MODEL_XY_PATH );
      auto xy_edge = std::make_shared< Model >( Display::WALLPANEL_MODEL_XY_EDGE_PATH );
      auto dr = std::make_shared< Model >( Display::WALLPANEL_MODEL_DR_PATH );
      XWallInstance::Piece = xy;
      XWallInstance::EdgePiece = xy_edge;
      YWallInstance::Piece = xy;
      YWallInstance::EdgePiece = xy_edge;
      DWallInstance::Piece = dr;
      RWallInstance::Piece = dr;

      // Setup camera
      camera.setRotationDirect( currentRotation );

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
      loadInfrastructure();
    }
    Display::MainGameState::~MainGameState() {
      XWallInstance::Piece.reset();
      YWallInstance::Piece.reset();

      DWallInstance::Piece.reset();
      RWallInstance::Piece.reset();
    }
    /**
     * Determine if a wall on a segment in this dimension exists.
     */
    bool Display::MainGameState::isWallDimensionPresent( std::string& frontPath, std::string& backPath, std::unique_ptr< Scripting::WallCell::Segment >& ptr ) {
      if( ptr ) {
        frontPath.assign( ptr->front.lock< std::string >( [ & ]( Scripting::Wallpaper& wallpaper ) { return wallpaper.imagePath; } ) );
        backPath.assign( ptr->back.lock< std::string >( [ & ]( Scripting::Wallpaper& wallpaper ) { return wallpaper.imagePath; } ) );
        return true;
      } else {
        return false;
      }
    }
    void Display::MainGameState::createFloorInstances() {
      floorInstanceCollection->clear();

      auto dimensions = floorMap.getDimensions();

      float xOrigin = -( (int)dimensions.x / 2 ) + 0.5f;
      float yOrigin = ( dimensions.y / 2 ) - 0.5f;

      for ( unsigned int zCounter = 0; zCounter != dimensions.levels; zCounter++ ) {
        for( unsigned int yCounter = 0; yCounter != dimensions.y; yCounter++ ) {
          for( unsigned int xCounter = 0; xCounter != dimensions.x; xCounter++ ) {

            glm::vec3 floorCoords( xOrigin + xCounter, yOrigin - yCounter, -10.0f - zCounter );

            Threading::Lockable< Scripting::Tile > tilePtr = floorMap.getItem( zCounter, xCounter, yCounter );

            if( tilePtr ) {
              // Create instance from the model, and change its material using the material cache
              std::shared_ptr< Instance > instance = std::make_shared< Instance >( floorModel, defaultShader.Program );

              Drawable& floorDrawable = instance->drawables.at( "Plane" );

              tilePtr.lock( [ & ]( Scripting::Tile& tile ) {
                floorDrawable.material = std::make_shared< Material >( TextureList{ texCache.get( tile.imagePath ) } );
              } );

              instance->setPosition( floorCoords );

              // The pointer to this floor tile goes into the floorInstanceCollection
              floorInstanceCollection->pushDirect( instance );
            } else {
              // There is no floor tile located here. Consequently, insert an empty Instance pointer here; it will be skipped on draw.
              floorInstanceCollection->pushDirect( std::shared_ptr< Instance >() );
            }

          }
        }
      }
    }
    void Display::MainGameState::createWallInstances() {
      wallInstanceCollection->clear();

      auto dimensions = wallMap.getDimensions();

      float xOrigin = -( (int)dimensions.x / 2 ) + 0.5f;
      float yOrigin = ( dimensions.y / 2 ) - 0.5f;

      for ( unsigned int zCounter = 0; zCounter != dimensions.levels; zCounter++ ) {
        for( unsigned int yCounter = 0; yCounter != dimensions.y; yCounter++ ) {
          for( unsigned int xCounter = 0; xCounter != dimensions.x; xCounter++ ) {

            // WALLS
            // Do not nudge any walls here (nudging will be the responsibility of MainGameState)
            // Nudging "X" means moving up in the Y dimension by 0.1
            // Nudging "Y" means moving left in the X dimension by 0.1

            glm::vec3 wallCenter( xOrigin + xCounter, yOrigin - yCounter, -10.0f - zCounter );

            Threading::Lockable< Scripting::WallCell > wallCellPtr = wallMap.getItem( zCounter, xCounter, yCounter );
            std::shared_ptr< Display::MainGameState::WallCellBundler > wallCellBundler;
            if( wallCellPtr ) {
              // Several different kinds of wall panel models depending on the type, and several kinds of orientations
              // If that pointer exists, at least one of these ifs will be fulfilled
              auto& bundler = getWallCellBundler( wallCellBundler );

              std::string frontPath;
              std::string backPath;

              // oh my fucking god i am so sorry about this
              if( wallCellPtr.lock< bool >( [ & ]( Scripting::WallCell& wallCell ) { return isWallDimensionPresent( frontPath, backPath, wallCell.x ); } ) ) {
                bundler.x = newXWallInstance( wallCenter.x, wallCenter.y, wallCenter.z, frontPath, backPath );
              }

              if( wallCellPtr.lock< bool >( [ & ]( Scripting::WallCell& wallCell ) { return isWallDimensionPresent( frontPath, backPath, wallCell.y ); } ) ) {
                // Rotations 0 and 1: Check for either yCounter equal to 0 (always place an edge piece) or [ yCounter - 1 ][ xCounter ]
                // Rotations 2 and 3: Check for either yCounter equal to dimensions.y - 2 (always place an edge piece), or [ yCounter + 1 ][ xCounter ]
                bool edge = false;

                auto checkNearbySegments = [ & ]( unsigned int adjusted_yCounter ) {
                  Threading::Lockable< Scripting::WallCell > adjacentCell = wallMap.getItem( zCounter, xCounter, adjusted_yCounter );

                  if( adjacentCell ) {
                    // We know *something* is here but is it the wall we need?
                    return adjacentCell.lock< bool >( []( Scripting::WallCell& wallCell ) { return !wallCell.y; } );
                  } else {
                    // We know nothing is here: edge piece.
                    return true;
                  }
                };

                switch( currentRotation ) {
                  case 0:
                  case 1:
                    edge = yCounter == 0 ? true : checkNearbySegments( yCounter - 1 );
                    break;
                  case 2:
                  case 3:
                  default:
                    edge = yCounter == dimensions.y - 2 ? true : checkNearbySegments( yCounter + 1 );
                }

                bundler.y = newYWallInstance( wallCenter.x, wallCenter.y, wallCenter.z, frontPath, backPath, edge );
              }

              if( wallCellPtr.lock< bool >( [ & ]( Scripting::WallCell& wallCell ) { return isWallDimensionPresent( frontPath, backPath, wallCell.d ); } ) ) {
                bundler.d = newDWallInstance( wallCenter.x, wallCenter.y, wallCenter.z, frontPath, backPath );
              }

              if( wallCellPtr.lock< bool >( [ & ]( Scripting::WallCell& wallCell ) { return isWallDimensionPresent( frontPath, backPath, wallCell.r ); } ) ) {
                bundler.r = newRWallInstance( wallCenter.x, wallCenter.y, wallCenter.z, frontPath, backPath );
              }
            }

            wallInstanceCollection->pushDirect( wallCellBundler );

          }
        }
      }
    }
    /**
     * Separated functions to get new wall instances
     */
    std::shared_ptr< XWallInstance > Display::MainGameState::newXWallInstance( float x, float y, float floorLevel, std::string& frontWallpaper, std::string& backWallpaper, bool edge ) {
      std::shared_ptr< XWallInstance > value = std::make_shared< XWallInstance >( defaultShader.Program, texCache, imageCache, edge );
      value->setPosition( glm::vec3( x, y + 0.9f, floorLevel ) );
      value->setRotationAngle( glm::radians( 180.0f ) );

      value->setWallpaper( frontWallpaper, backWallpaper );
      value->selectMaterial( currentRotation );

      return value;
    }
    std::shared_ptr< YWallInstance > Display::MainGameState::newYWallInstance( float x, float y, float floorLevel, std::string& frontWallpaper, std::string& backWallpaper, bool edge ) {
      std::shared_ptr< YWallInstance > value = std::make_shared< YWallInstance >( defaultShader.Program, texCache, imageCache, edge );

      // untested, sloppy code
      if( edge && ( currentRotation == 0 || currentRotation == 1 ) ) {
        value->setRotationAngle( glm::radians( 90.0f ) );
        value->setPosition( glm::vec3( x, y, floorLevel ) );
      } else {
        value->setRotationAngle( glm::radians( -90.0f ) );
        value->setPosition( glm::vec3( x - 0.9f, y, floorLevel ) );
      }

      value->setWallpaper( frontWallpaper, backWallpaper );
      value->selectMaterial( currentRotation );

      return value;
    }
    std::shared_ptr< DWallInstance > Display::MainGameState::newDWallInstance( float x, float y, float floorLevel, std::string& frontWallpaper, std::string& backWallpaper ) {
      std::shared_ptr< DWallInstance > value = std::make_shared< DWallInstance >( defaultShader.Program, texCache, imageCache );
      value->setPosition( glm::vec3( x, y, floorLevel ) );

      value->setWallpaper( frontWallpaper, backWallpaper );
      value->selectMaterial( currentRotation );

      return value;
    }
    std::shared_ptr< RWallInstance > Display::MainGameState::newRWallInstance( float x, float y, float floorLevel, std::string& frontWallpaper, std::string& backWallpaper ) {
      std::shared_ptr< RWallInstance > value = std::make_shared< RWallInstance >( defaultShader.Program, texCache, imageCache );
      value->setRotationAngle( glm::radians( -90.0f ) );
      value->setPosition( glm::vec3( x, y, floorLevel ) );

      value->setWallpaper( frontWallpaper, backWallpaper );
      value->selectMaterial( currentRotation );

      return value;
    }
    /**
     * Update wall edge segments; this basically means discarding existing wall segments built using createWallInstances and replacing them as-needed with edge segments.
     * See dev/wall/edgeSegments.md
     */
    void Display::MainGameState::updateWallEdgeSegments() {
      auto dimensions = wallInstanceCollection->getDimensions();

      float xOrigin = -( (int)dimensions.x / 2 ) + 0.5f;
      float yOrigin = ( dimensions.y / 2 ) - 0.5f;

      unsigned int xEdge = dimensions.x - 1;
      unsigned int yEdge = dimensions.y - 1;

      for ( unsigned int zCounter = 0; zCounter != dimensions.levels; zCounter++ ) {
        for( unsigned int yCounter = 0; yCounter != dimensions.y; yCounter++ ) {
          for( unsigned int xCounter = 0; xCounter != dimensions.x; xCounter++ ) {

            std::shared_ptr< Display::MainGameState::WallCellBundler > wallCellBundler = wallInstanceCollection->getItem( zCounter, xCounter, yCounter );

            if( wallCellBundler->y ) {
              // Y-AXIS SEGMENTS:
              // Rotations 0 & 1: Use edge piece if there is no segment at [ xCounter ][ yCounter - 1 ]. For edges of the board, at [ xCounter ][ 0 ], always use an edge piece.
              // If yCounter is equal to yEdge, don't check at all.
              // Rotations 2 & 3: Use edge piece if there is no segment at [ xCounter ][ yCounter + 1 ]. For edges of the board, at [ xCounter ][ yEdge ], always use an edge piece.
              // If yCounter is equal to 0, don't check at all.
            }


          }
        }
      }
    }
    void Display::MainGameState::loadInfrastructure() {
      auto dimensions = floorMap.getDimensions();
      floorInstanceCollection = std::make_unique< Containers::Collection3D< std::shared_ptr< Instance > > >( dimensions.levels, dimensions.x, dimensions.y );

      auto dimensionsWall = wallMap.getDimensions();
      wallInstanceCollection = std::make_unique< Containers::Collection3D< std::shared_ptr< Display::MainGameState::WallCellBundler > > >( dimensionsWall.levels, dimensionsWall.x, dimensionsWall.y );

      createFloorInstances();
      createWallInstances();

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

        // FIXME: This should not have to recalculate on every frame. Please revise this to precalculate/nudge only after a rotation.
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
              currentRotation = camera.rotateRight();
              remapWallTextures();
            }

            if( keyCode == KEY_ROTATE_LEFT ) {
              currentRotation = camera.rotateLeft();
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

    Display::SendInfrastructureCommand::SendInfrastructureCommand( Scripting::Lot& lot ) :
      rotation( lot.currentRotation ),
      floorMap( *lot.floorMap ),
      wallMap( *lot.wallMap ) {}
    void Display::SendInfrastructureCommand::execute( Graphics::Display& instance ) {
      instance.loadInfrastructure( rotation, floorMap, wallMap );
    }
  }
}
