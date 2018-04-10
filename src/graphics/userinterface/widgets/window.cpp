#include "graphics/userinterface/widgets/window.hpp"
#include "device/display/adapter/component/guicomponent.hpp"
#include "device/input/input.hpp"
#include "graphics/vector/renderer.hpp"
#include "tools/utility.hpp"
#include "log.hpp"
#include <GL/glew.h>
#include <glm/glm.hpp>

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {
      namespace Widgets {

        Window::Window( const std::string& id, const std::vector< std::string >& classes, const std::string& windowTitle ) : Element::Element( "Window", id, classes ), windowTitle( windowTitle ) {
          eventBundle.registerInputEvent( "mouse-down", [ & ]( Device::Input::Metadata event ) { onMouseDown( event ); } );
          eventBundle.registerInputEvent( "mouse-up", [ & ]( Device::Input::Metadata event ) { onMouseUp( event ); } );
        }

        std::shared_ptr< Window > Window::create( const std::string& id, const std::vector< std::string >& classes, const std::string& windowTitle ) {
          std::shared_ptr< Window > window( new Window( id, classes, windowTitle ) );

          return window;
        }

        void Window::onMouseDown( Device::Input::Metadata event ) {
          glm::ivec2 absPosition = getAbsolutePosition();
          glm::ivec2 origin = absPosition + getOrigin();
          glm::ivec2 corner = origin + getDimensions() - getOrigin();

          if( localStyle.get< bool >( "close-event" ) ) {
            if( Tools::Utility::intersect( event.mouseLocation, { corner.x - 15, origin.y, corner.x, origin.y + 20 } ) ) {
              Log::getInstance().debug( "Window::onMouseDown", "You clicked X in " + windowTitle );
            }

            if( Tools::Utility::intersect( event.mouseLocation, { origin.x, origin.y, corner.x - 15, origin.y + 20 } ) ) {
              glm::ivec2 differential = glm::ivec2( event.mouseLocation ) - origin;

              dragCallback = eventBundle.registerInputEvent( "mouse-moved", [ &, differential ]( Device::Input::Metadata moveEvent ) {
                localStyle.set< int >( "left", ( int ) moveEvent.mouseLocation.x - differential.x, false );
                localStyle.set< int >( "top", ( int ) moveEvent.mouseLocation.y - differential.y, false );

                if( std::shared_ptr< Element > parent = getParent() ) {
                  parent->paint();
                }
              } );
            }
          } else {
            if( Tools::Utility::intersect( event.mouseLocation, { origin.x, origin.y, corner.x, origin.y + 20 } ) ) {
              Log::getInstance().debug( "Window::onMouseDown", "You started dragging the window in " + windowTitle );
            }
          }
        }

        void Window::onMouseUp( Device::Input::Metadata event ) {
          if( dragCallback != -1 ) {
            eventBundle.unregisterInputEvent( "mouse-moved", dragCallback );
            dragCallback = -1;
          }
        }

        glm::ivec2 Window::getOrigin() {
          return { 5, 5 };
        }

        glm::ivec2 Window::getDimensions() {
          return { allocation[ 2 ] - 5, allocation[ 3 ] - 5 };
        }

        void Window::render( Graphics::Vector::Renderer& renderer ) {
          glm::ivec2 origin = getOrigin();
          glm::ivec2 dimensions = getDimensions();

          // Drop shadow
          // Left segment
          renderer.drawLinearGradient(
            { 0, origin.y, origin.x, dimensions.y },
            { origin.x, ( origin.y + ( ( dimensions.y - origin.y ) / 2 ) ), 0, ( origin.y + ( ( dimensions.y - origin.y ) / 2 ) ) },
            { 0, 0, 0, 128 },
            { 0, 0, 0, 0 }
          );
          // Top left corner
          renderer.drawScissored( { 0, 0, origin.x, origin.y }, [ & ]() {
            renderer.drawRadialGradient( origin, 0.05f, 4.95f, { 0, 0, 0, 128 }, { 0, 0, 0, 0 } );
          } );
          // Top segment
          renderer.drawLinearGradient(
            { 5, 0, dimensions.x, 5 },
            { ( origin.x + ( ( dimensions.x - origin.x ) / 2 ) ), 5, ( origin.x + ( ( dimensions.x - origin.x ) / 2 ) ), 0 },
            { 0, 0, 0, 128 },
            { 0, 0, 0, 0 }
          );
          // Top right corner
          renderer.drawScissored( { allocation[ 2 ] - origin.x, 0, allocation[ 2 ], origin.y }, [ & ]() {
            renderer.drawRadialGradient( { allocation[ 2 ] - origin.x, origin.y }, 0.05f, 4.95f, { 0, 0, 0, 128 }, { 0, 0, 0, 0 } );
          } );
          // Right segment
          renderer.drawLinearGradient(
            { allocation[ 2 ] - origin.x, origin.y, allocation[ 2 ], dimensions.y },
            { allocation[ 2 ] - origin.x, ( origin.y + ( ( dimensions.y - origin.y ) / 2 ) ), allocation[ 2 ], ( origin.y + ( ( dimensions.y - origin.y ) / 2 ) ) },
            { 0, 0, 0, 128 },
            { 0, 0, 0, 0 }
          );
          // Bottom segment
          renderer.drawLinearGradient(
            { origin.x, dimensions.y, allocation[ 2 ] - origin.x, allocation[ 3 ] },
            { ( ( dimensions.x - origin.x ) / 2 ), dimensions.y, ( ( dimensions.x - origin.x ) / 2 ), allocation[ 3 ] },
            { 0, 0, 0, 128 },
            { 0, 0, 0, 0 }
          );
          // Bottom left corner
          renderer.drawScissored( { 0, dimensions.y, origin.x, allocation[ 3 ] }, [ & ]() {
            renderer.drawRadialGradient( { origin.x, dimensions.y }, 0.05f, 4.95f, { 0, 0, 0, 128 }, { 0, 0, 0, 0 } );
          } );
          // Bottom right corner
          renderer.drawScissored( { dimensions.x, dimensions.y, allocation[ 2 ], allocation[ 3 ] }, [ & ]() {
            renderer.drawRadialGradient( { dimensions.x, dimensions.y }, 0.05f, 4.95f, { 0, 0, 0, 128 }, { 0, 0, 0, 0 } );
          } );

          // Background
          renderer.drawRect(
            { origin.x, origin.y, dimensions.x, dimensions.y },
            localStyle.get< glm::uvec4 >( "background-color" )
          );

          // Header
          renderer.drawRect(
            glm::uvec4{ origin.x, origin.y, dimensions.x, origin.y + 60 },
            localStyle.get< glm::uvec4 >( "color" )
          );

          // Header drop shadow
          renderer.drawLinearGradient(
            { origin.x, origin.y + 60, dimensions.x, origin.y + 65 },
            { ( ( dimensions.x - origin.x ) / 2 ), origin.y + 60, ( ( dimensions.x - origin.x ) / 2 ), origin.y + 65 },
            { 0, 0, 0, 128 },
            { 0, 0, 0, 0 }
          );

          // Titlebar
          renderer.drawRect(
            { origin.x, origin.y, dimensions.x, origin.y + 20 },
            { 0, 0, 0, 128 }
          );

          // Text
          double fontSize = localStyle.get< double >( "font-size" );
          renderer.drawText(
            localStyle.get< std::string >( "font" ),
            windowTitle,
            { origin.x + 10, origin.y + 27 + ( fontSize / 2 ) },
            localStyle.get< glm::uvec4 >( "font-color" ),
            fontSize
          );

          // Decorations
          if( localStyle.get< bool >( "close-event" ) ) {
            renderer.drawText( "fontawesome", "\uf00d", { dimensions.x - 15, origin.y + 10 }, localStyle.get< glm::uvec4 >( "font-color" ), 12.0 );
          }
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
