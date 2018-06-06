#include "state/householdgameplaystate.hpp"
#include "log.hpp"
#include "application.hpp"
#include "scripting/luastate.hpp"
#include "configmanager.hpp"
#include "eventmanager.hpp"
#include "device/display/adapter/component/worldrenderer.hpp"
#include "device/display/adapter/component/guicomponent.hpp"
#include "graphics/scenegraph/animation/animator.hpp"
#include "tools/utility.hpp"
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

    HouseholdGameplayState::HouseholdGameplayState( Application& application, const std::string& path ) : State::State( application ), engine( *this ), luaEventHelper( engine ) {
      setupDisplayDevice();
      setupInputDevice();

      engine.broadcastReadyEvent();
      engine.loadModpacks();

      // Load given lot from serialisation
      if( path != "" ) {
        load( Tools::Utility::fileToJson( path ) );
      }
    }

    HouseholdGameplayState::~HouseholdGameplayState() {
      application.getInputDevice().reset();
    }

    Json::Value HouseholdGameplayState::save() {
      return {};
    }

    void HouseholdGameplayState::load( const Json::Value& data ) {
      if( data != Json::Value::null ) {
        entityRegistry.load( data[ "registry" ] );
      }
    }

    void HouseholdGameplayState::setupDisplayDevice() {
      Device::Display::Adapter::Component::WorldRenderer& adapter = application
        .getDisplayDevice()
        .pushAdapter( std::make_unique< Device::Display::Adapter::Component::WorldRenderer >( application.getDisplayDevice() ) )
        .as< Device::Display::Adapter::Component::WorldRenderer >();

      world.emplace( adapter );
      infrastructure.emplace( adapter );

      application.getDisplayDevice().pushAdapter( std::make_unique< Device::Display::Adapter::Component::GuiComponent >( application.getDisplayDevice() ) );

      adapter.getCamera().setRotationDirect( 0 );
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

      Device::Input::Input& inputManager = application.getInputDevice();
      inputManager.reset();
      // NOTE: GUIComponent must capture events before WorldRenderer does in case it needs to eat the event using event.cancelAll()
      inputManager.registerInputEvent(
        sf::Event::KeyPressed,
        std::bind(
          &Device::Display::Adapter::Component::GuiComponent::keyPressed,
          &application.getDisplayDevice().getAdapterAt( GUI_ADAPTER ).as< Device::Display::Adapter::Component::GuiComponent >(),
          std::placeholders::_1
        )
      );
      inputManager.registerInputEvent(
        sf::Event::KeyReleased,
        std::bind(
          &Device::Display::Adapter::Component::GuiComponent::keyReleased,
          &application.getDisplayDevice().getAdapterAt( GUI_ADAPTER ).as< Device::Display::Adapter::Component::GuiComponent >(),
          std::placeholders::_1
        )
      );
      inputManager.registerInputEvent( sf::Event::KeyPressed, [ & ]( Device::Input::Metadata metadata ) {
        keyGroup.trigger( metadata.keyPressed );
      } );
      inputManager.registerInputEvent(
        sf::Event::MouseButtonPressed,
        std::bind(
          &Device::Display::Adapter::Component::GuiComponent::mousePressed,
          &application.getDisplayDevice().getAdapterAt( GUI_ADAPTER ).as< Device::Display::Adapter::Component::GuiComponent >(),
          std::placeholders::_1
        )
      );
      inputManager.registerInputEvent(
        sf::Event::MouseButtonReleased,
        std::bind(
          &Device::Display::Adapter::Component::GuiComponent::mouseReleased,
          &application.getDisplayDevice().getAdapterAt( GUI_ADAPTER ).as< Device::Display::Adapter::Component::GuiComponent >(),
          std::placeholders::_1
        )
      );
      inputManager.registerInputEvent(
        sf::Event::MouseMoved,
        std::bind(
          &Device::Display::Adapter::Component::GuiComponent::mouseMoved,
          &application.getDisplayDevice().getAdapterAt( GUI_ADAPTER ).as< Device::Display::Adapter::Component::GuiComponent >(),
          std::placeholders::_1
        )
      );

      luaEventHelper.connectInputDevice( inputManager );
    }

    Scripting::CoreEngine& HouseholdGameplayState::getEngine() {
      return engine;
    }

    void HouseholdGameplayState::update() {
      engine.update();

      auto& display = application.getDisplayDevice();
      display.update();

      auto& input = application.getInputDevice();
      input.update();
    }

  }
}
