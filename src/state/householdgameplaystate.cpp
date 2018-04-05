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
      application.getInputDevice().reset();
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
      keyGroup.registerSystemKey( Device::Input::Input::keyToString( KEY_ROTATE_RIGHT ), std::bind( &Graphics::Camera::rotateRight, &camera ) );
      keyGroup.registerSystemKey( Device::Input::Input::keyToString( KEY_ROTATE_LEFT ), std::bind( &Graphics::Camera::rotateLeft, &camera ) );
      keyGroup.registerSystemKey( Device::Input::Input::keyToString( KEY_UP ), std::bind( &Graphics::Camera::move, &camera, 0.0f, -0.1f, 0.0f ) );
      keyGroup.registerSystemKey( Device::Input::Input::keyToString( KEY_DOWN ), std::bind( &Graphics::Camera::move, &camera, 0.0f, 0.1f, 0.0f ) );
      keyGroup.registerSystemKey( Device::Input::Input::keyToString( KEY_LEFT ), std::bind( &Graphics::Camera::move, &camera, 0.1f, 0.0f, 0.0f ) );
      keyGroup.registerSystemKey( Device::Input::Input::keyToString( KEY_RIGHT ), std::bind( &Graphics::Camera::move, &camera, -0.1f, 0.0f, 0.0f ) );
      keyGroup.registerSystemKey( Device::Input::Input::keyToString( KEY_ZOOM_IN ), std::bind( &Graphics::Camera::zoomIn, &camera ) );
      keyGroup.registerSystemKey( Device::Input::Input::keyToString( KEY_ZOOM_OUT ), std::bind( &Graphics::Camera::zoomOut, &camera ) );

      keyGroup.registerSystemKey( Device::Input::Input::keyToString( sf::Keyboard::X ), [ & ]() {
        application.getDisplayDevice().getAdapterAt( RENDER3D_ADAPTER ).as< Device::Display::Adapter::Component::WorldRenderer >().placeObject( "floor", "floor1" );
      } );
      keyGroup.registerSystemKey( Device::Input::Input::keyToString( sf::Keyboard::C ), [ & ]() {
        auto animator = application.getDisplayDevice().getAdapterAt( RENDER3D_ADAPTER ).as< Device::Display::Adapter::Component::WorldRenderer >().getObject( "floor1" )
          ->findNearestAnimator();

        animator->setCurrentAnimation( "Armature|ArmatureAction" );
      } );
      keyGroup.registerSystemKey( Device::Input::Input::keyToString( sf::Keyboard::V ), [ & ]() {
        Log::getInstance().debug( "HouseholdGameplayState::setupInputDevice", "Trying this shit..." );

        application
          .getDisplayDevice()
          .getAdapterAt( GUI_ADAPTER )
          .as< Device::Display::Adapter::Component::GuiComponent >()
          .__testadd();

        Log::getInstance().debug( "HouseholdGameplayState::setupInputDevice", "Done" );
      } );
      keyGroup.registerSystemKey( Device::Input::Input::keyToString( sf::Keyboard::B ), [ & ]() {
        Log::getInstance().debug( "HouseholdGameplayState::setupInputDevice", "Applying style..." );

        application
          .getDisplayDevice()
          .getAdapterAt( GUI_ADAPTER )
          .as< Device::Display::Adapter::Component::GuiComponent >()
          .__teststyle();

        Log::getInstance().debug( "HouseholdGameplayState::setupInputDevice", "Done" );
      } );

      Device::Input::Input& inputManager = application.getInputDevice();
      inputManager.reset();
      inputManager.registerInputEvent( sf::Event::KeyPressed, [ & ]( Device::Input::Input::Metadata metadata ) {
        keyGroup.trigger( metadata.keyPressed );
      } );
    }

    Scripting::InfrastructureFactory& HouseholdGameplayState::getInfrastructureFactory() {
      return *infrastructureFactory;
    }

    void HouseholdGameplayState::update() {
      engine->update();

      auto& display = application.getDisplayDevice();
      display.update();

      auto& input = application.getInputDevice();
      input.update();
    }

  }
}
