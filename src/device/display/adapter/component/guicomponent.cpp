#include "device/display/adapter/component/guicomponent.hpp"
#include "device/display/display.hpp"
#include "graphics/userinterface/element.hpp"
#include "graphics/userinterface/widgets/layout.hpp"
#include "graphics/userinterface/propertylist.hpp"
#include "graphics/userinterface/drawable.hpp"
#include "configmanager.hpp"
#include "log.hpp"
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "graphics/userinterface/widgets/text.hpp"

namespace BlueBear {
  namespace Device {
    namespace Display {
      namespace Adapter {
        namespace Component {

          GuiComponent::GuiComponent( Device::Display::Display& display ) :
            Adapter::Adapter( display ),
            vector( display ),
            guiShader( "system/shaders/gui/vertex.glsl", "system/shaders/gui/fragment.glsl" ) {
              Graphics::UserInterface::Element::manager = this;

              rootElement = Graphics::UserInterface::Widgets::Layout::create( "", {} );

              rootElement->getPropertyList().set< int >( "top", 0 );
              rootElement->getPropertyList().set< int >( "left", 0 );
              rootElement->getPropertyList().set< int >( "width", ConfigManager::getInstance().getIntValue( "viewport_x" ) );
              rootElement->getPropertyList().set< int >( "height", ConfigManager::getInstance().getIntValue( "viewport_y" ) );
              rootElement->getPropertyList().set< Graphics::UserInterface::Gravity >( "gravity", Graphics::UserInterface::Gravity::TOP );
              rootElement->setAllocation( {
                0,
                0,
                ConfigManager::getInstance().getIntValue( "viewport_x" ),
                ConfigManager::getInstance().getIntValue( "viewport_y" )
              } );
            }

          // TODO: remove TEST code
          void GuiComponent::__testadd() {
            auto text = Graphics::UserInterface::Widgets::Text::create( "text", {}, "This is test text." );
            text->getPropertyList().set< glm::uvec4 >( "color", glm::uvec4{ 255, 255, 255, 255 } );
            text->getPropertyList().set< glm::uvec4 >( "background-color", glm::uvec4{ 64, 64, 64, 255 } );

            rootElement->addChild( text );
          }

          Graphics::Vector::Renderer& GuiComponent::getVectorRenderer() {
            return vector;
          }

          /**
           * Getting the feeling that the only reason we do this is because it's awkward to pass a "renderwindow" to inputmanager
           * when really it shouldn't be...
           */
          std::queue< sf::Event > GuiComponent::getEvents() {
            sf::RenderWindow& renderWindow = display.getRenderWindow();
            std::queue< sf::Event > events;

            sf::Event event;
            while( renderWindow.pollEvent( event ) ) {
              events.push( event );
            }

            return events;
          }

          void GuiComponent::nextFrame() {
            glDisable( GL_CULL_FACE );
            glDisable( GL_DEPTH_TEST );

            guiShader.use( true );
            rootElement->draw();

            glEnable( GL_CULL_FACE );
            glEnable( GL_DEPTH_TEST );
          }

        }
      }
    }
  }
}
