#include "graphics/display.hpp"
#include "containers/collection3d.hpp"
#include "graphics/gui/sfgroot.hpp"
#include "graphics/imagebuilder/imagesource.hpp"
#include "graphics/imagebuilder/pathimagesource.hpp"
#include "graphics/instance/instance.hpp"
#include "graphics/shader.hpp"
#include "graphics/model.hpp"
#include "graphics/drawable.hpp"
#include "graphics/camera.hpp"
#include "graphics/material.hpp"
#include "graphics/texture.hpp"
#include "graphics/wallcellbundler.hpp"
#include "graphics/widgetbuilder.hpp"
#include "scripting/lot.hpp"
#include "scripting/tile.hpp"
#include "scripting/engine.hpp"
#include "scripting/wallcell.hpp"
#include "scripting/wallpaper.hpp"
#include "localemanager.hpp"
#include "configmanager.hpp"
#include "eventmanager.hpp"
#include "tools/utility.hpp"
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

namespace BlueBear {
  namespace Graphics {

    const std::string Display::WALLPANEL_MODEL_XY_PATH = "system/models/wall/wall_new.dae";
    const std::string Display::WALLPANEL_MODEL_DR_PATH = "system/models/wall/diagwall.dae";
    const std::string Display::FLOOR_MODEL_PATH = "system/models/floor/floor.dae";

    Display::Display( lua_State* L, EventManager& eventManager ) : L( L ), eventManager( eventManager ) {
      // Get our settings out of the config manager
      x = ConfigManager::getInstance().getIntValue( "viewport_x" );
      y = ConfigManager::getInstance().getIntValue( "viewport_y" );

      // There must always be a defined State (avoid branch penalty/null check in tight loop)
      currentState = std::make_unique< IdleState >( *this );
    }

    Display::~Display() = default;

    void Display::openDisplay() {
      mainWindow.create( sf::VideoMode( x, y ), LocaleManager::getInstance().getString( "BLUEBEAR_WINDOW_TITLE" ), sf::Style::Close, sf::ContextSettings( 24, 8, 0, 3, 3 ) );

      mainWindow.resetGLStates();

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

    bool Display::update() {
      // Handle rendering
      currentState->execute();

      // Handle events
      sf::Event event;
      while( mainWindow.pollEvent( event ) ) {
        // This might be all we need for now
        switch( event.type ) {
          case sf::Event::Closed:
            mainWindow.close();
            return false;
          default:
            // Any event not handled by Display itself
            // is passed along to currentState
            currentState->handleEvent( event );
            break;
        }
      }

      return true;
    }

    /**
     * Given a lot, build floorInstanceCollection and translate the Tiles/Wallpanels to instances on the lot. Additionally, send the rotation status.
     */
    void Display::changeToMainGameState( unsigned int currentRotation, Containers::Collection3D< std::shared_ptr< Scripting::Tile > >& floorMap, Containers::Collection3D< std::shared_ptr< Scripting::WallCell > >& wallMap ) {

      std::unique_ptr< Display::MainGameState > mainGameStatePtr = std::make_unique< Display::MainGameState >( *this, currentRotation, floorMap, wallMap );

      currentState = std::move( mainGameStatePtr );
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
    Display::MainGameState::MainGameState( Display& instance, unsigned int currentRotation, Containers::Collection3D< std::shared_ptr< Scripting::Tile > >& floorMap, Containers::Collection3D< std::shared_ptr< Scripting::WallCell > >& wallMap ) :
      Display::State::State( instance ),
      L( instance.L ),
      defaultShader( Shader( "system/shaders/default_vertex.glsl", "system/shaders/default_fragment.glsl" ) ),
      camera( Camera( defaultShader.Program, instance.x, instance.y ) ),
      floorMap( floorMap ),
      wallMap( wallMap ),
      currentRotation( currentRotation ) {

      // Setup all system-level models used by this state
      loadIntrinsicModels();

      // Setup camera
      camera.setRotationDirect( currentRotation );

      setupGUI();
      submitLuaContributions();

      __debugInstances.emplace_back( *__debugModel, defaultShader.Program );

      // Moving much of Display::loadInfrastructure here
      loadInfrastructure();
    }
    Display::MainGameState::~MainGameState() {
      WallCellBundler::Piece.reset();
      WallCellBundler::DPiece.reset();
    }
    void Display::MainGameState::loadIntrinsicModels() {
      WallCellBundler::Piece = std::make_unique< Model >( Display::WALLPANEL_MODEL_XY_PATH );
      WallCellBundler::DPiece = std::make_unique< Model >( Display::WALLPANEL_MODEL_DR_PATH );

      floorModel = std::make_unique< Model >( Display::FLOOR_MODEL_PATH );
      __debugModel = std::make_unique< Model >( ConfigManager::getInstance().getValue( "__debug_file" ) );
    }
    /**
     * bluebear.gui and associated commands to create and manage windows
     */
    void Display::MainGameState::submitLuaContributions() {
      lua_getglobal( L, "bluebear" ); // bluebear

      lua_pushstring( L, "gui" ); // "gui" bluebear
      lua_newtable( L ); // {} "gui" bluebear

      lua_pushstring( L, "load_widgets" ); // string {} "gui" bluebear
      lua_pushlightuserdata( L, this ); // this string {} "gui" bluebear
      lua_pushcclosure( L, &Display::MainGameState::lua_loadXMLWidgets, 1 ); // closure string {} "gui" bluebear
      lua_settable( L, -3 ); // {} "gui" bluebear

      lua_pushstring( L, "rotate_left" );
      lua_pushlightuserdata( L, this );
      lua_pushcclosure( L, &Display::MainGameState::lua_rotateWorldLeft, 1 );
      lua_settable( L, -3 );

      lua_pushstring( L, "rotate_right" );
      lua_pushlightuserdata( L, this );
      lua_pushcclosure( L, &Display::MainGameState::lua_rotateWorldRight, 1 );
      lua_settable( L, -3 );

      lua_pushstring( L, "zoom_in" );
      lua_pushlightuserdata( L, this );
      lua_pushcclosure( L, &Display::MainGameState::lua_zoomIn, 1 );
      lua_settable( L, -3 );

      lua_pushstring( L, "zoom_out" );
      lua_pushlightuserdata( L, this );
      lua_pushcclosure( L, &Display::MainGameState::lua_zoomOut, 1 );
      lua_settable( L, -3 );

      // TODO: get_widget_by_class
      lua_pushstring( L, "get_widget_by_id" );
      lua_pushlightuserdata( L, this );
      lua_pushcclosure( L, &Display::MainGameState::lua_getWidgetByID, 1 );
      lua_settable( L, -3 );

      // XXX: Remove when demo branch is over
      lua_pushstring( L, "__internal__playanim1" );
      lua_pushlightuserdata( L, this );
      lua_pushcclosure( L, &Display::MainGameState::lua_playanim1, 1 );
      lua_settable( L, -3 );

      lua_pushstring( L, "__internal__playanim2" );
      lua_pushlightuserdata( L, this );
      lua_pushcclosure( L, &Display::MainGameState::lua_playanim2, 1 );
      lua_settable( L, -3 );

      lua_settable( L, -3 ); // bluebear

      lua_pop( L, 1 ); // EMPTY

      // Register internal sfg::Widget wrappers
      luaL_Reg elementFuncs[] = {
        { "get_widget_by_id", Display::MainGameState::lua_getWidgetByID },
        { "on", Display::MainGameState::lua_Widget_onEvent },
        { "__gc", Display::MainGameState::lua_Widget_gc },
        { NULL, NULL }
      };

      // TODO: Make sure these metatables in the registry don't fuck up the serialization save process!
      if( luaL_newmetatable( L, "bluebear_widget" ) ) { // metatable
        // If it returns 1, this metatable was newly created and has to get built.

        // Push the upvalue for this, we're gonna need it in some functions
        lua_pushlightuserdata( L, this ); // upvalue metatable

        luaL_setfuncs( L, elementFuncs, 1 ); // metatable

        lua_pushvalue( L, -1 ); // metatable metatable

        // HOLY SHIT THIS FUNCTION EXISTS?!
        lua_setfield( L, -2, "__index" ); // metatable
      }

      lua_pop( L, 1 ); // EMPTY
    }
    void Display::MainGameState::setupGUI() {
      gui.rootContainer = GUI::RootContainer::Create();
      gui.desktop.Add( gui.rootContainer );

      if( !gui.desktop.LoadThemeFromFile( ConfigManager::getInstance().getValue( "ui_theme" ) ) ) {
        Log::getInstance().warn( "Display::MainGameState::MainGameState", "ui_theme unable to load." );
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

            glm::vec3 floorCoords( xOrigin + xCounter, yOrigin - yCounter, zCounter * 2.0f );

            std::shared_ptr< Scripting::Tile > tilePtr = floorMap.getItem( zCounter, xCounter, yCounter );

            if( tilePtr ) {
              // Create instance from the model, and change its material using the material cache
              std::shared_ptr< Instance > instance = std::make_shared< Instance >( *floorModel, defaultShader.Program );

              Drawable& floorDrawable = *( instance->drawable );

              floorDrawable.material = std::make_shared< Material >( TextureList{ texCache.get( tilePtr->imagePath ) } );

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

      WallCellBundler::xOrigin = -( (int)dimensions.x / 2 ) + 0.5f;
      WallCellBundler::yOrigin = ( dimensions.y / 2 ) - 0.5f;

      for ( unsigned int zCounter = 0; zCounter != dimensions.levels; zCounter++ ) {
        for( unsigned int yCounter = 0; yCounter != dimensions.y; yCounter++ ) {
          for( unsigned int xCounter = 0; xCounter != dimensions.x; xCounter++ ) {

            std::shared_ptr< Scripting::WallCell > wallCellPtr = wallMap.getItem( zCounter, xCounter, yCounter );
            std::shared_ptr< WallCellBundler > wallCellBundler;
            if( wallCellPtr ) {
              // Several different kinds of wall panel models depending on the type, and several kinds of orientations
              // If that pointer exists, at least one of these ifs will be fulfilled
               wallCellBundler = std::make_shared< WallCellBundler >(
                 wallCellPtr,
                 *wallInstanceCollection,
                 glm::vec3( xCounter, yCounter, zCounter ),
                 currentRotation, texCache, imageCache, defaultShader.Program
               );
            }

            wallInstanceCollection->pushDirect( wallCellBundler );

          }
        }
      }
    }
    void Display::MainGameState::loadInfrastructure() {
      auto dimensions = floorMap.getDimensions();
      floorInstanceCollection = std::make_unique< Containers::Collection3D< std::shared_ptr< Instance > > >( dimensions.levels, dimensions.x, dimensions.y );

      auto dimensionsWall = wallMap.getDimensions();
      wallInstanceCollection = std::make_unique< Containers::Collection3D< std::shared_ptr< WallCellBundler > > >( dimensionsWall.levels, dimensionsWall.x, dimensionsWall.y );

      createFloorInstances();
      createWallInstances();

      Log::getInstance().info( "Display::MainGameState::loadInfrastructure", "Finished creating infrastructure instances." );
    }
    void Display::MainGameState::execute() {
      glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

      // Use default shader and position camera
      defaultShader.use();
      camera.position();

      // Draw some stuff while we refine the graphics engine
      for( auto& instance : __debugInstances ) {
        instance.drawEntity();
      }

      // Draw entities of each type
      // Floor & Walls with nudging
      auto length = floorInstanceCollection->getLength();
      for( auto i = 0; i != length; i++ ) {
        std::shared_ptr< Instance > floorInstance = floorInstanceCollection->getItemDirect( i );

        if( floorInstance ) {
          floorInstance->drawEntity();
        }
      }

      auto wallLength = wallInstanceCollection->getLength();
      for( auto i = 0; i != wallLength; i++ ) {
        std::shared_ptr< WallCellBundler > wallCellBundler = wallInstanceCollection->getItemDirect( i );

        if( wallCellBundler ) {
          wallCellBundler->render();
        }
      }

      processOsd();

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
      static int KEY_ZOOM_IN = ConfigManager::getInstance().getIntValue( "key_zoom_in" );
      static int KEY_ZOOM_OUT = ConfigManager::getInstance().getIntValue( "key_zoom_out" );

      gui.desktop.HandleEvent( event );

      // Main game has a few events mostly relating to camera
      switch( event.type ) {
        case sf::Event::KeyPressed:
          // absolutely disgusting
          {
            auto keyCode = event.key.code;

            /*
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
            */

            if( keyCode == KEY_ROTATE_RIGHT ) {
              currentRotation = camera.rotateRight();
              createWallInstances();
            }

            if( keyCode == KEY_ROTATE_LEFT ) {
              currentRotation = camera.rotateLeft();
              createWallInstances();
            }

            if( keyCode == KEY_UP ) {
              camera.move( 0.0f, -0.1f, 0.0f );
            }
            if( keyCode == KEY_DOWN ) {
              camera.move( 0.0f, 0.1f, 0.0f );
            }
            if( keyCode == KEY_LEFT ) {
              camera.move( 0.1f, 0.0f, 0.0f );
            }
            if( keyCode == KEY_RIGHT ) {
              camera.move( -0.1f, 0.0f, 0.0f );
            }
            if( keyCode == KEY_ZOOM_IN ) {
              camera.zoomIn();
            }
            if( keyCode == KEY_ZOOM_OUT ) {
              camera.zoomOut();
            }

            break;
          }
        default:
          break;
      }
    }
    void Display::MainGameState::processOsd() {
      glDisable( GL_DEPTH_TEST );
      gui.desktop.Update( gui.clock.restart().asSeconds() );
      instance.sfgui.Display( instance.mainWindow );
      glEnable( GL_DEPTH_TEST );
    }
    int Display::MainGameState::lua_loadXMLWidgets( lua_State* L ) {

      Display::MainGameState* self = ( Display::MainGameState* )lua_touserdata( L, lua_upvalueindex( 1 ) );

      // Stack contains string to path of XML file

      if( lua_isstring( L, -1 ) ) {
        std::string path( lua_tostring( L, -1 ) );

        try {
          // Create a WidgetBuilder and dump its widgets into the root container
          // This should run in the engine objectLoop stage, and it will be caught on subsequent render
          WidgetBuilder builder( path );
          std::vector< std::shared_ptr< sfg::Widget > > widgets = builder.getWidgets();
          for( auto& widget : widgets ) {
            self->gui.rootContainer->Add( widget );
          }
        } catch( ... ) {
          Log::getInstance().error( "Display::MainGameState::lua_loadXMLWidgets", "Failed to create a WidgetBuilder for path " + path );
        }
      } else {
        Log::getInstance().warn( "Display::MainGameState::lua_loadXMLWidgets", "Argument 1 provided to bluebear.gui.load_widgets is not a string." );
      }

      lua_pop( L, 1 ); // EMPTY

      return 0;
    }
    int Display::MainGameState::lua_zoomIn( lua_State* L ) {
      Display::MainGameState* self = ( Display::MainGameState* )lua_touserdata( L, lua_upvalueindex( 1 ) );

      self->camera.zoomIn();
    }
    int Display::MainGameState::lua_zoomOut( lua_State* L ) {
      Display::MainGameState* self = ( Display::MainGameState* )lua_touserdata( L, lua_upvalueindex( 1 ) );

      self->camera.zoomOut();
    }
    int Display::MainGameState::lua_rotateWorldLeft( lua_State* L ) {
      Display::MainGameState* self = ( Display::MainGameState* )lua_touserdata( L, lua_upvalueindex( 1 ) );

      self->currentRotation = self->camera.rotateLeft();
      self->createWallInstances();
    }
    int Display::MainGameState::lua_rotateWorldRight( lua_State* L ) {
      Display::MainGameState* self = ( Display::MainGameState* )lua_touserdata( L, lua_upvalueindex( 1 ) );

      self->currentRotation = self->camera.rotateRight();
      self->createWallInstances();
    }
    int Display::MainGameState::lua_getWidgetByID( lua_State* L ) {

      std::string selector;

      // called from bluebear.gui.get_widget_by_id
      if( lua_isstring( L, -1 ) ) {
        selector = std::string( lua_tostring( L, -1 ) );
      } else {
        Log::getInstance().warn( "Display::MainGameState::lua_getWidgetByID", "Argument provided to get_widget_by_id was not a string." );
        return 0;
      }

      Display::MainGameState* self = ( Display::MainGameState* )lua_touserdata( L, lua_upvalueindex( 1 ) );
      std::shared_ptr< sfg::Widget > parentWidget;
      if( lua_gettop( L ) == 1 ) {
        parentWidget = self->gui.rootContainer;
      } else {
        std::shared_ptr< sfg::Widget >* widgetPtr = *( ( std::shared_ptr< sfg::Widget >** ) luaL_checkudata( L, 1, "bluebear_widget" ) );
        parentWidget = *widgetPtr;
      }

      // Holy moly is this going to get confusing quick
      // I also have the least amount of confidence in this code you could possibly imagine
      std::shared_ptr< sfg::Widget > widget = parentWidget->GetWidgetById( selector );
      if( widget ) {
        Log::getInstance().debug( "Display::MainGameState::lua_getWidgetByID", "Creating a userdata for the found element " + selector );
        LuaElement** userData = ( LuaElement** )lua_newuserdata( L, sizeof( LuaElement* ) ); // userdata
        *userData = new LuaElement();

        ( **userData ).widget = widget;

        luaL_getmetatable( L, "bluebear_widget" ); // metatable userdata
        lua_setmetatable( L, -2 ); // userdata

        return 1;
      } else {
        std::string error = std::string( "Could not find widget with ID " ) + selector;
        return luaL_error( L, error.c_str() );
      }
    }
    int Display::MainGameState::lua_Widget_gc( lua_State* L ) {
      LuaElement* widgetPtr = *( ( LuaElement** ) luaL_checkudata( L, 1, "bluebear_widget" ) );

      // Destroy the std::shared_ptr< sfg::Widget >. This should decrease the reference count by one.
      delete widgetPtr;

      return 0;
    }
    int Display::MainGameState::lua_Widget_onEvent( lua_State* L ) {
      LuaElement** userData = ( LuaElement** ) luaL_checkudata( L, 1, "bluebear_widget" );

      Display::MainGameState* self = ( Display::MainGameState* )lua_touserdata( L, lua_upvalueindex( 1 ) );

      // function "event" self

      if( lua_isstring( L, -2 ) ) {
        const char* eventType = lua_tostring( L, -2 );

        switch( Tools::Utility::hash( eventType ) ) {
          case Tools::Utility::hash( "click" ):
            {
              LuaElement& element = **userData;

              // Create the bucket for this widget if it doesn't exist, otherwise, return a new bucket
              SignalMap& signalMap = self->masterSignalMap[ element.widget.get() ];

              // If there's a previous sfg::Widget::OnLeftClick registered for this widget instance, unref and kill it
              auto pair = signalMap.find( sfg::Widget::OnLeftClick );
              if( pair != signalMap.end() ) {
                // Un-ref this element we're about to erase
                luaL_unref( L, LUA_REGISTRYINDEX, pair->second );
              }

              // Track the master reference
              // Unref this if the pointer is ever removed!
              LuaReference masterReference = signalMap[ sfg::Widget::OnLeftClick ] = luaL_ref( L, LUA_REGISTRYINDEX ); // "event" self

              element.widget->GetSignal( sfg::Widget::OnLeftClick ).Connect( [ L, self, masterReference ]() {
                // Create new "disposable" reference that will get ferried through
                lua_rawgeti( L, LUA_REGISTRYINDEX, masterReference ); // object
                self->instance.eventManager.UI_ACTION_EVENT.trigger( luaL_ref( L, LUA_REGISTRYINDEX ) ); // EMPTY
              } );

              lua_pushboolean( L, true ); // true "event" self
              return 1; // true
            }
            break;
          default:
            Log::getInstance().warn( "Display::MainGameState::lua_Widget_onEvent", "Invalid event type specified: " + std::string( eventType ) );
        }
      } else {
        Log::getInstance().warn( "Display::MainGameState::lua_Widget_onEvent", "Invalid event passed to on()" );
      }

      return 0;
    }
    // XXX: Remove after demo branch
    int Display::MainGameState::lua_playanim1( lua_State* L ) {
      Display::MainGameState* self = ( Display::MainGameState* )lua_touserdata( L, lua_upvalueindex( 1 ) );

      auto& inst = self->__debugInstances[ 0 ].children[ "Cube.001" ];
      inst->setAnimation( "Cube.001|Rotate45" );
    }
    int Display::MainGameState::lua_playanim2( lua_State* L ) {
      Display::MainGameState* self = ( Display::MainGameState* )lua_touserdata( L, lua_upvalueindex( 1 ) );

      auto& inst = self->__debugInstances[ 0 ].children[ "Cube.001" ];
      inst->setAnimation( "Cube.001|RotateNeg45" );
    }
  }
}
