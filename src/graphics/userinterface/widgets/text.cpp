#include "graphics/userinterface/widgets/text.hpp"
#include "graphics/userinterface/propertylist.hpp"
#include "device/display/adapter/component/guicomponent.hpp"
#include "graphics/vector/renderer.hpp"
#include "graphics/userinterface/drawable.hpp"
#include <cstdlib>

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {
      namespace Widgets {

        Text::Text( const std::string& id, const std::vector< std::string >& classes, const std::string& innerText ) : Element::Element( "Text", id, classes ), innerText( innerText ) {}

        std::shared_ptr< Text > Text::create( const std::string& id, const std::vector< std::string >& classes, const std::string& innerText ) {
          std::shared_ptr< Text > text( new Text( id, classes, innerText ) );

          return text;
        }

        void Text::render( Graphics::Vector::Renderer& renderer ) {
          double fontSize = localStyle.get< double >( "font-size" );

          renderer.drawRect(
            glm::uvec4{ 0, 0, allocation[ 2 ], allocation[ 3 ] },
            localStyle.get< glm::uvec4 >( "background-color" )
          );

          renderer.drawText(
            localStyle.get< std::string >( "font" ),
            innerText,
            glm::uvec2{ ( allocation[ 2 ] / 2 ) - ( textSpan / 2 ), fontSize / 2 },
            localStyle.get< glm::uvec4 >( "color" ),
            fontSize
          );
        }

        void Text::calculate() {
          int padding = localStyle.get< int >( "padding" );
          double fontSize = localStyle.get< double >( "font-size" );
          glm::vec4 size = manager->getVectorRenderer().getTextSizeParams( localStyle.get< std::string >( "font" ), innerText, fontSize );
          textSpan = size[ 2 ];

          requisition = glm::uvec2{
            ( padding * 2 ) + textSpan,
            ( padding * 2 ) + fontSize
          };
        }

        void Text::reflow( bool selectorsInvalidated ) {
          if( auto parent = getParent() ) {
            parent->reflow( selectorsInvalidated );
          } else {
            Element::reflow( selectorsInvalidated );
          }
        }

        void Text::setText( const std::string& text, bool doReflow ) {
          innerText = text;

          if( doReflow ) {
            reflow();
          }
        }

        std::string Text::getText() {
          return innerText;
        }

      }
    }
  }
}
