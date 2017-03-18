#include "graphics/display.hpp"
#include "containers/collection3d.hpp"
#include "graphics/gui/sfgroot.hpp"
#include "graphics/gui/luaelement.hpp"
#include "graphics/gui/luaguicontext.hpp"
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
#include "graphics/shaderinstancebundle.hpp"
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
#include <glm/ext.hpp>
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
      inputManager( instance.eventManager ),
      defaultShader( Shader( "system/shaders/default_vertex.glsl", "system/shaders/default_fragment.glsl" ) ),
      camera( Camera( instance.x, instance.y ) ),
      floorMap( floorMap ),
      wallMap( wallMap ),
      currentRotation( currentRotation ) {

      // Setup all system-level models used by this state
      loadIntrinsicModels();

      // Setup camera
      camera.setRotationDirect( currentRotation );

      setupGUI();
      submitLuaContributions();

      dynamicInstances.emplace_back( camera, "system/shaders/default_vertex.glsl", "system/shaders/default_fragment.glsl" );

      dynamicInstances[ 0 ].instances.emplace_back( *__debugModel );
      dynamicInstances[ 0 ].instances[ 0 ].setPosition( glm::vec3( 2.5f, 2.5f, 0.0f ) );

      // Moving much of Display::loadInfrastructure here
      loadInfrastructure();

      // Register keyboard events
      registerEvents();
    }
    Display::MainGameState::~MainGameState() {
      WallCellBundler::Piece.reset();
      WallCellBundler::DPiece.reset();
    }
    void Display::MainGameState::registerEvents() {
      // Lay out some statics
      static sf::Keyboard::Key KEY_PERSPECTIVE = ( sf::Keyboard::Key ) ConfigManager::getInstance().getIntValue( "key_switch_perspective" );
      static sf::Keyboard::Key KEY_ROTATE_RIGHT = ( sf::Keyboard::Key ) ConfigManager::getInstance().getIntValue( "key_rotate_right" );
      static sf::Keyboard::Key KEY_ROTATE_LEFT = ( sf::Keyboard::Key ) ConfigManager::getInstance().getIntValue( "key_rotate_left" );
      static sf::Keyboard::Key KEY_UP = ( sf::Keyboard::Key ) ConfigManager::getInstance().getIntValue( "key_move_up" );
      static sf::Keyboard::Key KEY_DOWN = ( sf::Keyboard::Key ) ConfigManager::getInstance().getIntValue( "key_move_down" );
      static sf::Keyboard::Key KEY_LEFT = ( sf::Keyboard::Key ) ConfigManager::getInstance().getIntValue( "key_move_left" );
      static sf::Keyboard::Key KEY_RIGHT = ( sf::Keyboard::Key ) ConfigManager::getInstance().getIntValue( "key_move_right" );
      static sf::Keyboard::Key KEY_ZOOM_IN = ( sf::Keyboard::Key ) ConfigManager::getInstance().getIntValue( "key_zoom_in" );
      static sf::Keyboard::Key KEY_ZOOM_OUT = ( sf::Keyboard::Key ) ConfigManager::getInstance().getIntValue( "key_zoom_out" );

      inputManager.listen( KEY_ROTATE_RIGHT, [ & ]() {
        currentRotation = camera.rotateRight();
        createWallInstances();
      } );

      inputManager.listen( KEY_ROTATE_LEFT, [ & ]() {
        currentRotation = camera.rotateLeft();
        createWallInstances();
      } );

      inputManager.listen( KEY_UP, [ & ]() {
        camera.move( 0.0f, -0.1f, 0.0f );
      } );

      inputManager.listen( KEY_DOWN, [ & ]() {
        camera.move( 0.0f, 0.1f, 0.0f );
      } );

      inputManager.listen( KEY_LEFT, [ & ]() {
        camera.move( 0.1f, 0.0f, 0.0f );
      } );

      inputManager.listen( KEY_RIGHT, [ & ]() {
        camera.move( -0.1f, 0.0f, 0.0f );
      } );

      inputManager.listen( KEY_ZOOM_IN, [ & ]() {
        camera.zoomIn();
      } );

      inputManager.listen( KEY_ZOOM_OUT, [ & ]() {
        camera.zoomOut();
      } );
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

      // TODO: Deprecate this in favor of create_gui_context( 'path/to/layout.xml' )
      lua_pushstring( L, "load_widgets" ); // string {} "gui" bluebear
      lua_pushlightuserdata( L, this ); // this string {} "gui" bluebear
      lua_pushcclosure( L, &Display::MainGameState::lua_loadXMLWidgets, 1 ); // closure string {} "gui" bluebear
      lua_settable( L, -3 ); // {} "gui" bluebear

      lua_pushstring( L, "create_gui_context" );
      lua_pushlightuserdata( L, this );
      lua_pushcclosure( L, &Display::MainGameState::lua_createGUIContext, 1 );
      lua_settable( L, -3 );

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

      lua_pushstring( L, "get_widget_by_id" );
      lua_pushlightuserdata( L, this );
      lua_pushcclosure( L, &GUI::LuaElement::lua_getWidgetByID, 1 );
      lua_settable( L, -3 );

      lua_pushstring( L, "get_widgets_by_class" );
      lua_pushlightuserdata( L, this );
      lua_pushcclosure( L, &GUI::LuaElement::lua_getWidgetsByClass, 1 );
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
        { "get_widget_by_id", GUI::LuaElement::lua_getWidgetByID },
        { "get_widgets_by_class", GUI::LuaElement::lua_getWidgetsByClass },
        { "on", GUI::LuaElement::lua_onEvent },
        { "off", GUI::LuaElement::lua_offEvent },
        { "get_content", GUI::LuaElement::lua_getText },
        { "set_content", GUI::LuaElement::lua_setText },
        { "get_property", GUI::LuaElement::lua_getProperty },
        { "set_property", GUI::LuaElement::lua_setProperty },
        { "set_image", GUI::LuaElement::lua_setImage },
        { "__gc", GUI::LuaElement::lua_gc },
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

      luaL_Reg guiContextFuncs[] = {
        { "find_by_id", GUI::LuaGUIContext::lua_findById },
        { "find_by_class", GUI::LuaGUIContext::lua_findByClass },
        { "__gc", GUI::LuaGUIContext::lua_gc },
        { NULL, NULL }
      };

      if( luaL_newmetatable( L, "bluebear_gui_context" ) ) { // metatable
        lua_pushlightuserdata( L, this ); // upvalue metatable
        luaL_setfuncs( L, guiContextFuncs, 1 ); // metatable
        lua_pushvalue( L, -1 ); // metatable metatable
        lua_setfield( L, -2, "__index" ); // metatable
      }

      lua_pop( L, 1 ); // EMPTY
    }
    void Display::MainGameState::setupGUI() {
      GUI::LuaElement::masterSignalMap.clear();

      gui.rootContainer = GUI::RootContainer::Create();

      inputManager.setRootContainer( gui.rootContainer );

      if( !gui.desktop.LoadThemeFromFile( ConfigManager::getInstance().getValue( "ui_theme" ) ) ) {
        Log::getInstance().warn( "Display::MainGameState::MainGameState", "ui_theme unable to load." );
      }

      // SFGUI will not set a constant on a signal until it is used
      // Sidestep this irritating behaviour here
      sfg::Widget::OnStateChange = sfg::Signal::GetGUID();
  		sfg::Widget::OnGainFocus = sfg::Signal::GetGUID();
  		sfg::Widget::OnLostFocus = sfg::Signal::GetGUID();

  		sfg::Widget::OnExpose = sfg::Signal::GetGUID();

  		sfg::Widget::OnSizeAllocate = sfg::Signal::GetGUID();
  		sfg::Widget::OnSizeRequest = sfg::Signal::GetGUID();

  		sfg::Widget::OnMouseEnter = sfg::Signal::GetGUID();
  		sfg::Widget::OnMouseLeave = sfg::Signal::GetGUID();
  		sfg::Widget::OnMouseMove = sfg::Signal::GetGUID();
  		sfg::Widget::OnMouseLeftPress = sfg::Signal::GetGUID();
  		sfg::Widget::OnMouseRightPress = sfg::Signal::GetGUID();
  		sfg::Widget::OnMouseLeftRelease = sfg::Signal::GetGUID();
  		sfg::Widget::OnMouseRightRelease = sfg::Signal::GetGUID();

  		sfg::Widget::OnLeftClick = sfg::Signal::GetGUID();
  		sfg::Widget::OnRightClick = sfg::Signal::GetGUID();

  		sfg::Widget::OnKeyPress = sfg::Signal::GetGUID();
  		sfg::Widget::OnKeyRelease = sfg::Signal::GetGUID();
  		sfg::Widget::OnText = sfg::Signal::GetGUID();
      sfg::Entry::OnTextChanged = sfg::Signal::GetGUID();
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
              std::shared_ptr< Instance > instance = std::make_shared< Instance >( *floorModel );

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
                 currentRotation, texCache, imageCache
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


      camera.position();

      // USES DEFAULT SHADER
      // Draw entities of each type
      // Floor & Walls with nudging
      defaultShader.use();
      camera.sendToShader();
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

      // USES VARYING SHADERS
      for( ShaderInstanceBundle& bundle : dynamicInstances ) {
        bundle.drawInstances();
      }

      processOsd();

      instance.mainWindow.display();
    }
    void Display::MainGameState::handleEvent( sf::Event& event ) {
      // Useful for some metadata in event handling
      currentEvent = &event;

      gui.desktop.HandleEvent( event );
      inputManager.handleEvent( event );

      // Never leave a pointer dangling
      currentEvent = nullptr;
    }
    void Display::MainGameState::processOsd() {
      glDisable( GL_DEPTH_TEST );
      gui.desktop.Update( gui.clock.restart().asSeconds() );
      instance.sfgui.Display( instance.mainWindow );
      glEnable( GL_DEPTH_TEST );
    }
    ImageCache& Display::MainGameState::getImageCache() {
      return imageCache;
    }
    /**
     * XXX: Deprecated
     */
    int Display::MainGameState::lua_loadXMLWidgets( lua_State* L ) {

      Display::MainGameState* self = ( Display::MainGameState* )lua_touserdata( L, lua_upvalueindex( 1 ) );

      // Stack contains string to path of XML file

      if( lua_isstring( L, -1 ) ) {
        std::string path( lua_tostring( L, -1 ) );

        try {
          // Create a WidgetBuilder and dump its widgets into the root container
          // This should run in the engine objectLoop stage, and it will be caught on subsequent render
          WidgetBuilder builder( self->instance.eventManager, self->imageCache, path );
          std::vector< std::shared_ptr< sfg::Widget > > widgets = builder.getWidgets();
          for( auto& widget : widgets ) {
            // Add to root container for proper bookkeeping
            self->gui.rootContainer->Add( widget );

            // Add directly to desktop for proper Z-ordering
            self->gui.desktop.Add( widget );
          }
        } catch( const std::exception& e ) {
          Log::getInstance().error( "Display::MainGameState::lua_loadXMLWidgets", "Failed to create a WidgetBuilder for path " + path + ": " + e.what() );
        }
      } else {
        Log::getInstance().warn( "Display::MainGameState::lua_loadXMLWidgets", "Argument 1 provided to bluebear.gui.load_widgets is not a string." );
      }

      return 0;
    }
    int Display::MainGameState::lua_createGUIContext( lua_State* L ) {
      Display::MainGameState* self = ( Display::MainGameState* )lua_touserdata( L, lua_upvalueindex( 1 ) );

      if( lua_isstring( L, -1 ) ) {
        std::string path( lua_tostring( L, -1 ) );

        try {
          // Create a WidgetBuilder and dump its widgets into the root container
          // This should run in the engine objectLoop stage, and it will be caught on subsequent render
          GUI::LuaGUIContext** userData = ( GUI::LuaGUIContext** )lua_newuserdata( L, sizeof( GUI::LuaGUIContext* ) ); // userdata "arg"
          WidgetBuilder builder( self->instance.eventManager, self->imageCache, path );
          *userData = new GUI::LuaGUIContext( self->gui.desktop, builder );

          luaL_getmetatable( L, "bluebear_gui_context" ); // metatable userdata "arg"
          lua_setmetatable( L, -2 ); // userdata "arg"
          return 1;
        } catch( const std::exception& e ) {
          Log::getInstance().error(
            "Display::MainGameState::lua_createGUIContext",
            "Failed to create a WidgetBuilder for path " + path + ": " + e.what()
          );
        }
      } else {
        Log::getInstance().warn(
          "Display::MainGameState::lua_createGUIContext",
          "Argument 1 provided to bluebear.gui.create_gui_context is not a string."
        );
      }

      return 0;
    }
    int Display::MainGameState::lua_zoomIn( lua_State* L ) {
      Display::MainGameState* self = ( Display::MainGameState* )lua_touserdata( L, lua_upvalueindex( 1 ) );

      self->camera.zoomIn();
      return 0;
    }
    int Display::MainGameState::lua_zoomOut( lua_State* L ) {
      Display::MainGameState* self = ( Display::MainGameState* )lua_touserdata( L, lua_upvalueindex( 1 ) );

      self->camera.zoomOut();
      return 0;
    }
    int Display::MainGameState::lua_rotateWorldLeft( lua_State* L ) {
      Display::MainGameState* self = ( Display::MainGameState* )lua_touserdata( L, lua_upvalueindex( 1 ) );

      self->currentRotation = self->camera.rotateLeft();
      self->createWallInstances();
      return 0;
    }
    int Display::MainGameState::lua_rotateWorldRight( lua_State* L ) {
      Display::MainGameState* self = ( Display::MainGameState* )lua_touserdata( L, lua_upvalueindex( 1 ) );

      self->currentRotation = self->camera.rotateRight();
      self->createWallInstances();
      return 0;
    }
    // XXX: Remove after demo branch
    int Display::MainGameState::lua_playanim1( lua_State* L ) {
      Display::MainGameState* self = ( Display::MainGameState* )lua_touserdata( L, lua_upvalueindex( 1 ) );

      self->dynamicInstances[ 0 ].instances[ 0 ].setAnimation( "Armature|ArmatureAction.002" );
      return 0;
    }
    int Display::MainGameState::lua_playanim2( lua_State* L ) {
      Display::MainGameState* self = ( Display::MainGameState* )lua_touserdata( L, lua_upvalueindex( 1 ) );
      return 0;
    }
  }
}
