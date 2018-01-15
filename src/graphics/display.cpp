#include "graphics/display.hpp"
#include "containers/collection3d.hpp"
#include "graphics/gui/sfgroot.hpp"
#include "graphics/gui/luaelement.hpp"
#include "graphics/gui/luadesktopfunctions.hpp"
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
#include "graphics/modelloader.hpp"
#include "scripting/luakit/gchelper.hpp"
#include "scripting/lot.hpp"
#include "scripting/engine.hpp"
#include "scripting/wallcell.hpp"
#include "models/wallpaper.hpp"
#include "localemanager.hpp"
#include "configmanager.hpp"
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

    Display::Display( BlueBear::State::State& s, Scripting::Engine* e ) : BlueBear::State::Substate( s ), engine( e ), L( e->L ) {
      // Get our settings out of the config manager
      x = ConfigManager::getInstance().getIntValue( "viewport_x" );
      y = ConfigManager::getInstance().getIntValue( "viewport_y" );

      // There must always be a defined State (avoid branch penalty/null check in tight loop)
      currentState = std::make_unique< IdleState >( *this );
    }

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
    void Display::changeToMainGameState( unsigned int currentRotation, Containers::Collection3D< std::shared_ptr< Models::Tile > >& floorMap, Containers::Collection3D< std::shared_ptr< Scripting::WallCell > >& wallMap ) {

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
    Display::MainGameState::MainGameState( Display& instance, unsigned int currentRotation, Containers::Collection3D< std::shared_ptr< Models::Tile > >& floorMap, Containers::Collection3D< std::shared_ptr< Scripting::WallCell > >& wallMap ) :
      Display::State::State( instance ),
      L( instance.L ),
      inputManager( Input::InputManager( instance.L ) ),
      camera( Camera( instance.x, instance.y ) ),
      floorMap( floorMap ),
      wallMap( wallMap ),
      currentRotation( currentRotation ) {

      // Lay out default shader
      registeredShaders[ "default" ] = std::make_shared< Shader >( "system/shaders/default_vertex.glsl", "system/shaders/default_fragment.glsl" );

      // Setup all system-level models used by this state
      loadIntrinsicModels();

      // Setup camera
      camera.setRotationDirect( currentRotation );

      setupGUI();
      submitLuaContributions();

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
    }
    /**
     * bluebear.gui and associated commands to create and manage windows
     */
    void Display::MainGameState::submitLuaContributions() {
      lua_getglobal( L, "bluebear" ); // bluebear

      lua_pushstring( L, "gui" ); // "gui" bluebear
      lua_newtable( L ); // {} "gui" bluebear

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

      // Replacement for most of the LuaGUIContext stuff
      lua_pushstring( L, "find_by_id" );
      lua_pushlightuserdata( L, this );
      lua_pushcclosure( L, &GUI::LuaDesktopFunctions::lua_findById, 1 );
      lua_settable( L, -3 );

      lua_pushstring( L, "find_by_class" );
      lua_pushlightuserdata( L, this );
      lua_pushcclosure( L, &GUI::LuaDesktopFunctions::lua_findByClass, 1 );
      lua_settable( L, -3 );

      lua_pushstring( L, "add_from_path" );
      lua_pushlightuserdata( L, this );
      lua_pushcclosure( L, &GUI::LuaDesktopFunctions::lua_addXMLFromPath, 1 );
      lua_settable( L, -3 );

      lua_pushstring( L, "add" );
      lua_pushlightuserdata( L, this );
      lua_pushcclosure( L, &GUI::LuaDesktopFunctions::lua_add, 1 );
      lua_settable( L, -3 );

      lua_pushstring( L, "remove" );
      lua_pushlightuserdata( L, this );
      lua_pushcclosure( L, &GUI::LuaDesktopFunctions::lua_removeWidget, 1 );
      lua_settable( L, -3 );

      lua_pushstring( L, "create" );
      lua_pushlightuserdata( L, this );
      lua_pushcclosure( L, &GUI::LuaDesktopFunctions::lua_createWidget, 1 );
      lua_settable( L, -3 );

      lua_pushstring( L, "load_theme" );
      lua_pushlightuserdata( L, this );
      lua_pushcclosure( L, &GUI::LuaDesktopFunctions::lua_loadThemeFromFile, 1 );
      lua_settable( L, -3 );

      lua_settable( L, -3 ); // bluebear

      lua_getfield( L, -1, "event" ); // event bluebear

      lua_pushstring( L, "register_key" );
      lua_pushlightuserdata( L, this );
      lua_pushcclosure( L, &Input::InputManager::lua_registerScriptKey, 1 );
      lua_settable( L, -3 );

      lua_pushstring( L, "unregister_key" );
      lua_pushlightuserdata( L, this );
      lua_pushcclosure( L, &Input::InputManager::lua_unregisterScriptKey, 1 );
      lua_settable( L, -3 );

      lua_pop( L, 1 ); // bluebear

      lua_pushstring( L, "world" ); // "world" bluebear
      lua_newtable( L ); // {} "world" bluebear

      lua_pushstring( L, "get_model_loader" );
      lua_pushlightuserdata( L, this );
      lua_pushcclosure( L, &ModelLoaderHelper::lua_createModelLoader, 1 );
      lua_settable( L, -3 );

      lua_settable( L, -3 ); // bluebear

      lua_pop( L, 1 ); // EMPTY

      // Register lua crap for the ModelLoader
      luaL_Reg modelLoaderFuncs[] = {
        { "load_model", ModelLoaderHelper::lua_loadModel },
        { "get_instance", ModelLoaderHelper::lua_getInstance },
        { "__gc", ModelLoaderHelper::lua_gc },
        { NULL, NULL }
      };

      if( luaL_newmetatable( L, "bluebear_model_loader" ) ) { // metatable

        lua_pushlightuserdata( L, this ); // upvalue metatable
        luaL_setfuncs( L, modelLoaderFuncs, 1 ); // metatable
        lua_pushvalue( L, -1 ); // metatable metatable
        lua_setfield( L, -2, "__index" ); // metatable

      }

      lua_pop( L, 1 ); // EMPTY

      // Register lua crap for the LuaInstanceHelper
      luaL_Reg luaInstanceHelperFuncs[] = {
        { "get_anim_list", LuaInstanceHelper::lua_getAnimList },
        { "set_anim", LuaInstanceHelper::lua_setAnimation },
        { "get_anim", LuaInstanceHelper::lua_getAnimation },
        { "pause_anim", LuaInstanceHelper::lua_pauseAnimation },
        { "is_anim_paused", LuaInstanceHelper::lua_isAnimationPaused },
        { "set_anim_frame", LuaInstanceHelper::lua_setAnimationFrame },
        { "get_anim_duration", LuaInstanceHelper::lua_getAnimationDuration },
        { "__gc", LuaInstanceHelper::lua_gc },
        { NULL, NULL }
      };

      if( luaL_newmetatable( L, "bluebear_graphics_instance" ) ) { // metatable

        lua_pushlightuserdata( L, this ); // upvalue metatable
        luaL_setfuncs( L, luaInstanceHelperFuncs, 1 ); // metatable
        lua_pushvalue( L, -1 ); // metatable metatable
        lua_setfield( L, -2, "__index" ); // metatable

      }

      lua_pop( L, 1 ); // EMPTY

      // Register internal sfg::Widget wrappers
      luaL_Reg elementFuncs[] = {
        { "find_by_id", GUI::LuaElement::lua_getWidgetByID },
        { "find_by_class", GUI::LuaElement::lua_getWidgetsByClass },
        { "find_pseudo", GUI::LuaElement::lua_getPseudoElements },
        { "add", GUI::LuaElement::lua_add },
        { "remove", GUI::LuaElement::lua_widgetRemove },
        { "on", GUI::LuaElement::lua_onEvent },
        { "off", GUI::LuaElement::lua_offEvent },
        { "get_name", GUI::LuaElement::lua_getName },
        { "get_content", GUI::LuaElement::lua_getText },
        { "set_content", GUI::LuaElement::lua_setText },
        { "get_property", GUI::LuaElement::lua_getProperty },
        { "set_property", GUI::LuaElement::lua_setProperty },
        { "set_image", GUI::LuaElement::lua_setImage },
        { "get_style", GUI::LuaElement::lua_getStyleProperty },
        { "get_children", GUI::LuaElement::lua_getChildElements },
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

      Scripting::LuaKit::GcHelper::initialize( L );
    }
    void Display::MainGameState::setupGUI() {
      GUI::LuaElement::masterSignalMap.clear();
      GUI::LuaElement::masterAttrMap.clear();

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

            std::shared_ptr< Models::Tile > tilePtr = floorMap.getItem( zCounter, xCounter, yCounter );

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
      registeredShaders[ "default" ]->use();
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

      drawWorldInstances();

      processOsd();

      instance.mainWindow.display();
    }
    void Display::MainGameState::drawWorldInstances() {
      std::unordered_map< std::shared_ptr< Shader >, std::vector< std::shared_ptr< Instance > > > bundles;

      // For each entity, dig through its world_objects field (if present) and retrieve all the instances that need to be drawn
      for( LuaReference entity : instance.engine->objects ) {
        lua_rawgeti( L, LUA_REGISTRYINDEX, entity ); // table
        lua_getfield( L, -1, "world_objects" ); // world_objects table

        if( lua_istable( L, -1 ) ) {
          unsigned int size = lua_rawlen( L, -1 );
          for( int i = 1; i <= size; i++ ) {
            // Screen for bluebear_graphics_instance
            lua_rawgeti( L, -1, i ); // item world_objects table

            LuaInstanceHelper** helperPtr = ( LuaInstanceHelper** ) luaL_testudata( L, 3, "bluebear_graphics_instance" );
            if( helperPtr ) {
              LuaInstanceHelper* helper = *helperPtr;

              bundles[ helper->shader ].push_back( helper->instance );
            }

            lua_pop( L, 1 ); // world_objects table
          }
        }

        lua_pop( L, 2 ); // EMPTY
      }

      // Now draw each
      for( auto& pair : bundles ) {
        // pair.first is the shader. camera will be updated automatically with every shader change
        pair.first->use();

        for( std::shared_ptr< Instance > individualInstance : pair.second ) {
          individualInstance->drawEntity();
        }
      }
    }
    void Display::MainGameState::handleEvent( sf::Event& event ) {
      // Useful for some metadata in event handling
      currentEvent = &event;

      gui.desktop.HandleEvent( event );
      inputManager.handleEvent( event );

      if( event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Z ) {
        World::Renderer testRenderer;

        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        testRenderer.loadPathsParallel( {
          { "12vert", "dev/box/12vert.fbx" },
          { "armaturebox", "dev/box/armaturebox.fbx" },
          { "box", "dev/box/box.fbx" },
          { "box_no_anim", "dev/box/box_no_anim.fbx" },
          { "cylinder", "dev/box/cylinder.fbx" },
          { "even_simpler_2", "dev/box/even_simpler_2.fbx" },
          { "even_simpler", "dev/box/even_simpler.fbx" },
          { "mini_knight", "dev/box/mini_knight.fbx" }
        } );
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

        Log::getInstance().debug(
          "Z-key renderer debugging",
          std::to_string( std::chrono::duration_cast< std::chrono::milliseconds >( end - start ).count() ) + "ms"
        );
      }

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
    Input::InputManager& Display::MainGameState::getInputManager() {
      return inputManager;
    }
    std::map< std::string, std::shared_ptr< Shader > >& Display::MainGameState::getRegisteredShaders() {
      return registeredShaders;
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
  }
}
