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

        Window::Window( const std::string& id, const std::vector< std::string >& classes, const std::string& windowTitle ) : Element::Element( "Window", id, classes ) {
          eventBundle.registerInputEvent( "mouse-down", [ & ]( Device::Input::Metadata event ) { onMouseDown( event ); } );
          eventBundle.registerInputEvent( "mouse-up", [ & ]( Device::Input::Metadata event ) { onMouseUp( event ); } );
        }

        std::shared_ptr< Window > Window::create( const std::string& id, const std::vector< std::string >& classes, const std::string& windowTitle ) {
          std::shared_ptr< Window > window( new Window( id, classes, windowTitle ) );

          // Add shadow element
          auto decoration = WindowDecoration::create( windowTitle );
          window->addChild( decoration, false );

          return window;
        }

        void Window::onMouseDown( Device::Input::Metadata event ) {
          glm::ivec2 absPosition = getAbsolutePosition();
          glm::ivec2 origin = absPosition + getOrigin();
          glm::ivec2 corner = origin + getDimensions() - getOrigin();

          // Bring to top before we do anything else
          if( std::shared_ptr< Element > parent = getParent() ) {
            std::vector< std::shared_ptr< Element > > sortedChildren = parent->getChildren();

            // Sort if not already on top
            std::shared_ptr< Element > self = shared_from_this();
            if( sortedChildren.back() != self ) {
              // Free items only
              std::vector< std::shared_ptr< Element > > freeOnly;
              std::copy_if( sortedChildren.begin(), sortedChildren.end(), std::back_inserter( freeOnly ), []( std::shared_ptr< Element > item ){
                return item->getPropertyList().get< Placement >( "placement" ) == Placement::FREE;
              } );

              freeOnly.erase( std::remove( freeOnly.begin(), freeOnly.end(), self ), freeOnly.end() );
              freeOnly.push_back( self );

              // Fix all the local-z-order values
              unsigned int i = 1;
              for( std::shared_ptr< Element > item : freeOnly ) {
                item->getPropertyList().set< int >( "local-z-order", i, false );
                i++;
              }

              // Finally, reflow parent
              parent->paint();
            }
          }

          if( localStyle.get< bool >( "close-event" ) ) {
            if( Tools::Utility::intersect( event.mouseLocation, { corner.x - 15, origin.y, corner.x, origin.y + 20 } ) ) {
              return onCloseClick( event );
            }

            if( localStyle.get< bool >( "draggable" ) && Tools::Utility::intersect( event.mouseLocation, { origin.x, origin.y, corner.x - 15, origin.y + 20 } ) ) {
              manager->startDrag( shared_from_this(), event.mouseLocation - glm::ivec2{ localStyle.get< int >( "left" ), localStyle.get< int >( "top" ) } );
            }
          } else {
            if( Tools::Utility::intersect( event.mouseLocation, { origin.x, origin.y, corner.x, origin.y + 20 } ) ) {
              manager->startDrag( shared_from_this(), event.mouseLocation - glm::ivec2{ localStyle.get< int >( "left" ), localStyle.get< int >( "top" ) } );
            }
          }
        }

        void Window::onMouseUp( Device::Input::Metadata event ) {

        }

        void Window::onCloseClick( Device::Input::Metadata event ) {
          // TODO
        }

        glm::ivec2 Window::getOrigin() {
          return { 5, 5 };
        }

        glm::ivec2 Window::getDimensions() {
          return { allocation[ 2 ] - 5, allocation[ 3 ] - 5 };
        }

        void Window::positionAndSizeChildren() {
          // It better be there or else we're segfaulting
          std::shared_ptr< Element > decoration = *std::find_if( children.begin(), children.end(), [ & ]( std::shared_ptr< Element > child ) {
            return child->hasClass( "-bb-shadow-windowdecoration" );
          } );

          glm::ivec2 origin = getOrigin();
          decoration->setAllocation( { origin.x, origin.y, allocation[ 2 ] - 10, 65 }, false );
          // Decoration comes above local flow element
          decoration->getPropertyList().set< int >( "local-z-order", 2, false );

          for( std::shared_ptr< Element > child : children ) {
            if( child != decoration ) {
              if( child->getPropertyList().get< Placement >( "placement" ) == Placement::FLOW ) {
                // The single element
                child->setAllocation( { origin.x, origin.y + 60, allocation[ 2 ] - 10, allocation[ 3 ] - 70 } );
                child->getPropertyList().set< int >( "local-z-order", 1, false );
              } else {
                // Floating buttons, etc.
              }
            }
          }
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
        }

        void Window::calculate() {
          requisition = glm::uvec2{
            localStyle.get< int >( "width" ),
            localStyle.get< int >( "height" )
          };
        }

        // Window decoration shadow element

        WindowDecoration::WindowDecoration( const std::string& windowTitle ) : Element::Element( "WindowDecoration", "", { "-bb-shadow-windowdecoration" } ), windowTitle( windowTitle ) {
          shadow = true;
        }

        std::shared_ptr< WindowDecoration > WindowDecoration::create( const std::string& windowTitle ) {
          std::shared_ptr< WindowDecoration > windowDecoration( new WindowDecoration( windowTitle ) );

          return windowDecoration;
        }

        void WindowDecoration::calculate() {
          glm::vec4 size = manager->getVectorRenderer().getTextSizeParams( localStyle.get< std::string >( "font" ), windowTitle, localStyle.get< double >( "font-size" ) );
          textSpan = size[ 2 ];

          // Nothing else to calculate - width and height are known when positioned.
          requisition = { 0, 0 };
        }

        void WindowDecoration::render( Graphics::Vector::Renderer& renderer ) {
          if( std::shared_ptr< Element > parent = getParent() ) {
            glm::ivec2 origin = { 0, 0 };
            glm::ivec2 dimensions = { allocation[ 2 ], 65 };

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
            if( parent->getPropertyList().get< bool >( "close-event" ) ) {
              renderer.drawText( "fontawesome", "\uf00d", { dimensions.x - 15, origin.y + 10 }, localStyle.get< glm::uvec4 >( "font-color" ), 12.0 );
            }
          } else {
            Log::getInstance().error( "WindowDecoration::render", "Shadow element <WindowDecoration> should belong to an element but it does not!" );
          }
        }

      }
    }
  }
}
