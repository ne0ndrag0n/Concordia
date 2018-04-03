#include "graphics/userinterface/widgets/window.hpp"
#include "device/display/adapter/component/guicomponent.hpp"
#include "graphics/vector/renderer.hpp"
#include "log.hpp"
#include <glm/glm.hpp>

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {
      namespace Widgets {

        Window::Window( const std::string& id, const std::vector< std::string >& classes, const std::string& windowTitle ) : Element::Element( "Window", id, classes ), windowTitle( windowTitle ) {}

        std::shared_ptr< Window > Window::create( const std::string& id, const std::vector< std::string >& classes, const std::string& windowTitle ) {
          std::shared_ptr< Window > window( new Window( id, classes, windowTitle ) );

          return window;
        }

        void Window::render( Graphics::Vector::Renderer& renderer ) {
          // Background
          renderer.drawRect(
            glm::uvec4{ 0, 0, allocation[ 2 ], allocation[ 3 ] },
            localStyle.get< glm::uvec4 >( "background-color" )
          );

          // Header
          renderer.drawRect(
            glm::uvec4{ 0, 0, allocation[ 2 ], 60 },
            localStyle.get< glm::uvec4 >( "color" )
          );

          // Titlebar
          renderer.drawRect(
            glm::uvec4{ 0, 0, allocation[ 2 ], 20 },
            glm::uvec4{ 0, 0, 0, 128 }
          );

          // Text
          double fontSize = localStyle.get< double >( "font-size" );
          renderer.drawText(
            localStyle.get< std::string >( "font" ),
            windowTitle,
            glm::uvec2{ 10, 27 + ( fontSize / 2 ) },
            localStyle.get< glm::uvec4 >( "font-color" ),
            fontSize
          );

          // Decorations
          renderer.drawText( "fontawesome", "\uf00d", glm::uvec2{ allocation[ 2 ] - 15, 10 }, localStyle.get< glm::uvec4 >( "font-color" ), 12.0 );
        }

        void Window::calculate() {
          glm::vec4 size = manager->getVectorRenderer().getTextSizeParams( localStyle.get< std::string >( "font" ), windowTitle, localStyle.get< double >( "font-size" ) );
          textSpan = size[ 2 ];

          requisition = glm::uvec2{
            localStyle.get< int >( "width" ),
            localStyle.get< int >( "height" )
          };
        }

      }
    }
  }
}
