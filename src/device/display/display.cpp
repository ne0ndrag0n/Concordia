#include "device/display/display.hpp"
#include "device/display/adapter/adapter.hpp"
#include "configmanager.hpp"
#include "localemanager.hpp"
#include "log.hpp"
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/WindowStyle.hpp>
#include <SFML/Window/Context.hpp>
#include <GL/glew.h>

namespace BlueBear {
  namespace Device {
    namespace Display {

      Display::Display() :
        dimensions( glm::vec2{ ConfigManager::getInstance().getIntValue( "viewport_x" ), ConfigManager::getInstance().getIntValue( "viewport_y" ) } ) {
        window.create(
          sf::VideoMode( dimensions.x, dimensions.y ),
          LocaleManager::getInstance().getString( "BLUEBEAR_WINDOW_TITLE" ),
          sf::Style::Close,
          getDefaultContextSettings()
        );

        // Initialize OpenGL using GLEW
        glewExperimental = true;
        auto glewStatus = glewInit();
        if( glewStatus != GLEW_OK ) {
          Log::getInstance().error( "Display::Display", "FATAL: glewInit() did NOT return GLEW_OK! (" + std::string( ( const char* ) glewGetErrorString( glewStatus ) ) + ")" );
          exit( 1 );
        }

        // Set sync on window by these params:
        // vsync_limiter_overview = true or fps_overview
        if( ConfigManager::getInstance().getBoolValue( "vsync_limiter_overview" ) == true ) {
          window.setVerticalSyncEnabled( true );
        } else {
          window.setFramerateLimit( ConfigManager::getInstance().getIntValue( "fps_overview" ) );
        }

        glViewport( 0, 0, dimensions.x, dimensions.y );
        glEnable( GL_DEPTH_TEST );
        glEnable( GL_CULL_FACE );
        glEnable( GL_BLEND );
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
      }

      Display::~Display() {}

      sf::ContextSettings Display::getDefaultContextSettings() const {
        return sf::ContextSettings( 24, 8, 0, 3, 3, sf::ContextSettings::Core );
      }

      sf::RenderWindow& Display::getRenderWindow() {
        return window;
      }

      const glm::uvec2& Display::getDimensions() const {
        return dimensions;
      }

      Adapter::Adapter& Display::pushAdapter( std::unique_ptr< Adapter::Adapter > adapter ) {
        return *adapters.emplace_back( std::move( adapter ) );
      }

      Adapter::Adapter& Display::getAdapterAt( unsigned int index ) {
        return *adapters.at( index );
      }

      void Display::executeOnSecondaryContext( std::function< void() > closure ) {
        {
          sf::Context context( getDefaultContextSettings(), dimensions.x, dimensions.y ); // calls setActive( true ) on itself, or should anyway
          closure();
        }

        window.setActive( true );
      }

      void Display::executeOnSecondaryContext( sf::Context& context, std::function< void() > closure ) {
        context.setActive( true );

        closure();

        window.setActive( true );
      }

      void Display::reset() {
        adapters.clear();
      }

      void Display::update() {
        glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        for( std::unique_ptr< Adapter::Adapter >& adapter : adapters ) {
          if( adapter ) {
            adapter->nextFrame();
          }
        }

        window.display();
      }

    }
  }
}
