#include "graphics/userinterface/widgets/text.hpp"
#include "graphics/userinterface/propertylist.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {
      namespace Widgets {

        void Text::render( Device::Display::Adapter::Component::GuiComponent& manager ) {

        }

        void Text::calculate() {
          glm::ivec2 total{ localStyle.get< int >( "width" ), localStyle.get< int >( "height" ) };
          int padding = localStyle.get< int >( "padding" );
          int len = innerText.size();
          int fontSize = localStyle.get< int >( "font-size" );

          if( !valueIsLiteral( total.x ) ) {
            total.x = ( padding * 2 ) + ( fontSize * len );
          }

          if( !valueIsLiteral( total.y ) ) {
            total.y = ( padding * 2 ) + fontSize;
          }

          requisition = bindCalculations( total );
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
