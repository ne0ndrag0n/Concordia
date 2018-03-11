#include "graphics/userinterface/widgets/text.hpp"
#include "graphics/userinterface/propertylist.hpp"
#include "device/display/adapter/component/guicomponent.hpp"
#include "graphics/vector/renderer.hpp"
#include "graphics/userinterface/drawable.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {
      namespace Widgets {

        Text::Text( const std::string& id, const std::vector< std::string >& classes, const std::string& innerText ) : Element::Element( "Text", id, classes ), innerText( innerText ) {}

        std::shared_ptr< Text > Text::create( const std::string& id, const std::vector< std::string >& classes, const std::string& innerText ) {
          std::shared_ptr< Text > text( new Text( id, classes, innerText ) );

          return text;
        }

        void Text::render( Device::Display::Adapter::Component::GuiComponent& manager ) {
          glm::uvec2 absolutePosition = getAbsolutePosition();
          double fontSize = localStyle.get< double >( "font-size" );

          drawable = std::make_unique< UserInterface::Drawable >(
            manager.getVectorRenderer().createTexture(
              glm::uvec2{ allocation[ 2 ], allocation[ 3 ] },
              [ & ]( Graphics::Vector::Renderer& renderer ) {
                renderer.drawText(
                  localStyle.get< std::string >( "font" ),
                  innerText,
                  glm::uvec2{ 0, fontSize / 2 },
                  localStyle.get< glm::uvec4 >( "color" ),
                  fontSize
                );
              }
            ),
            absolutePosition.x,
            absolutePosition.y,
            allocation[ 2 ],
            allocation[ 3 ]
          );
        }

        void Text::calculate() {
          int padding = localStyle.get< int >( "padding" );
          int len = innerText.size();
          double fontSize = localStyle.get< double >( "font-size" );

          requisition = glm::uvec2{
            ( padding * 2 ) + ( fontSize * len ),
            ( padding * 2 ) + fontSize
          };
        }

        void Text::setText( const std::string& text ) {
          innerText = text;
        }

        std::string Text::getText() {
          return innerText;
        }

      }
    }
  }
}
