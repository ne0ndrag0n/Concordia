#include "state/householdgameplaystate.hpp"
#include "log.hpp"
#include "application.hpp"
#include "scripting/lot.hpp"
#include "scripting/infrastructurefactory.hpp"
#include "scripting/engine.hpp"
#include "scripting/luastate.hpp"
#include "configmanager.hpp"
#include "eventmanager.hpp"
#include "device/display/adapter/component/worldrenderer.hpp"
#include "device/display/adapter/component/guicomponent.hpp"
#include "device/input/input.hpp"
#include "graphics/scenegraph/animation/animator.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>
#include <functional>
#include <queue>
#include <map>

// Remove these after dev
#include "graphics/scenegraph/transform.hpp"
#include "graphics/scenegraph/model.hpp"
#include "graphics/vector/renderer.hpp"

namespace BlueBear {
  namespace State {

    HouseholdGameplayState::HouseholdGameplayState( Application& application ) : State::State( application ) {
      setupEngine();
      setupDisplayDevice();
      setupInputDevice();
      eventManager.LUA_STATE_READY.trigger( engine->lua );
    }

    HouseholdGameplayState::~HouseholdGameplayState() {
      application.getInputDevice() = Device::Input::Input();
    }

    void HouseholdGameplayState::setupEngine() {
      infrastructureFactory = std::make_unique< Scripting::InfrastructureFactory >();

      engine = std::make_unique< Scripting::Engine >( *this );
      if( !engine->submitLuaContributions() ) {
        Log::getInstance().error( "HouseholdGameplayState::HouseholdGameplayState", "Failed to load BlueBear!" );
        throw EngineLoadFailureException();
      }

      if( !engine->loadLot( "lots/lot01.json" ) ) {
        Log::getInstance().error( "HouseholdGameplayState::HouseholdGameplayState", "Failed to load demo lot!" );
        throw LotNotFoundException();
      }

      L = engine->L;
    }

    void HouseholdGameplayState::setupDisplayDevice() {
      Device::Display::Adapter::Component::WorldRenderer& adapter = application
        .getDisplayDevice()
        .pushAdapter( std::make_unique< Device::Display::Adapter::Component::WorldRenderer >( application.getDisplayDevice() ) )
        .as< Device::Display::Adapter::Component::WorldRenderer >();

      application.getDisplayDevice().pushAdapter( std::make_unique< Device::Display::Adapter::Component::GuiComponent >( application.getDisplayDevice() ) );

      adapter.getCamera().setRotationDirect( engine->currentLot->currentRotation );
      adapter.loadPathsParallel( {
        { "floor", "dev/box/armaturebox.fbx" }
      } );
    }

    void HouseholdGameplayState::setupInputDevice() {
      sf::Keyboard::Key KEY_PERSPECTIVE = ( sf::Keyboard::Key ) ConfigManager::getInstance().getIntValue( "key_switch_perspective" );
      sf::Keyboard::Key KEY_ROTATE_RIGHT = ( sf::Keyboard::Key ) ConfigManager::getInstance().getIntValue( "key_rotate_right" );
      sf::Keyboard::Key KEY_ROTATE_LEFT = ( sf::Keyboard::Key ) ConfigManager::getInstance().getIntValue( "key_rotate_left" );
      sf::Keyboard::Key KEY_UP = ( sf::Keyboard::Key ) ConfigManager::getInstance().getIntValue( "key_move_up" );
      sf::Keyboard::Key KEY_DOWN = ( sf::Keyboard::Key ) ConfigManager::getInstance().getIntValue( "key_move_down" );
      sf::Keyboard::Key KEY_LEFT = ( sf::Keyboard::Key ) ConfigManager::getInstance().getIntValue( "key_move_left" );
      sf::Keyboard::Key KEY_RIGHT = ( sf::Keyboard::Key ) ConfigManager::getInstance().getIntValue( "key_move_right" );
      sf::Keyboard::Key KEY_ZOOM_IN = ( sf::Keyboard::Key ) ConfigManager::getInstance().getIntValue( "key_zoom_in" );
      sf::Keyboard::Key KEY_ZOOM_OUT = ( sf::Keyboard::Key ) ConfigManager::getInstance().getIntValue( "key_zoom_out" );

      Graphics::Camera& camera = application.getDisplayDevice().getAdapterAt( RENDER3D_ADAPTER ).as< Device::Display::Adapter::Component::WorldRenderer >().getCamera();
      Device::Input::Input& inputManager = ( application.getInputDevice() = Device::Input::Input() );
      inputManager.listen( KEY_ROTATE_RIGHT, std::bind( &Graphics::Camera::rotateRight, &camera ) );
      inputManager.listen( KEY_ROTATE_LEFT, std::bind( &Graphics::Camera::rotateLeft, &camera ) );
      inputManager.listen( KEY_UP, std::bind( &Graphics::Camera::move, &camera, 0.0f, -0.1f, 0.0f ) );
      inputManager.listen( KEY_DOWN, std::bind( &Graphics::Camera::move, &camera, 0.0f, 0.1f, 0.0f ) );
      inputManager.listen( KEY_LEFT, std::bind( &Graphics::Camera::move, &camera, 0.1f, 0.0f, 0.0f ) );
      inputManager.listen( KEY_RIGHT, std::bind( &Graphics::Camera::move, &camera, -0.1f, 0.0f, 0.0f ) );
      inputManager.listen( KEY_ZOOM_IN, std::bind( &Graphics::Camera::zoomIn, &camera ) );
      inputManager.listen( KEY_ZOOM_OUT, std::bind( &Graphics::Camera::zoomOut, &camera ) );

      inputManager.listen( sf::Keyboard::X, [ & ]() {
        application.getDisplayDevice().getAdapterAt( RENDER3D_ADAPTER ).as< Device::Display::Adapter::Component::WorldRenderer >().placeObject( "floor", "floor1" );
      } );
      inputManager.listen( sf::Keyboard::C, [ & ]() {
        auto animator = application.getDisplayDevice().getAdapterAt( RENDER3D_ADAPTER ).as< Device::Display::Adapter::Component::WorldRenderer >().getObject( "floor1" )
          ->findNearestAnimator();

        animator->setCurrentAnimation( "Armature|ArmatureAction" );
      } );
      inputManager.listen( sf::Keyboard::V, [ & ]() {
        Log::getInstance().debug( "HouseholdGameplayState::setupInputDevice", "Trying this context shit..." );

        application
          .getDisplayDevice()
          .getAdapterAt( GUI_ADAPTER )
          .as< Device::Display::Adapter::Component::GuiComponent >()
          .getVectorRenderer()
          .createTexture( glm::uvec2{ 100, 100 }, [ & ]( Graphics::Vector::Renderer& renderer ) {
            renderer.drawRect( glm::uvec4{ 10, 10, 10, 10 }, glm::uvec4{ 10, 0, 0, 255 } );
          } );

        Log::getInstance().debug( "HouseholdGameplayState::setupInputDevice", "Back to the original context" );
      } );
    }

    Scripting::InfrastructureFactory& HouseholdGameplayState::getInfrastructureFactory() {
      return *infrastructureFactory;
    }

    void HouseholdGameplayState::update() {
      engine->update();

      auto& display = application.getDisplayDevice();
      display.update();

      // pull and use events
      std::queue< sf::Event > guiEvents = display.getAdapterAt( GUI_ADAPTER ).as< Device::Display::Adapter::Component::GuiComponent >().getEvents();
      while( !guiEvents.empty() ) {
        sf::Event& event = guiEvents.front();
        switch( event.type ) {
          case sf::Event::Closed:
            application.close();
            return;
          default:
            application.getInputDevice().handleEvent( event );
        }

        guiEvents.pop();
      }
    }

  }
}