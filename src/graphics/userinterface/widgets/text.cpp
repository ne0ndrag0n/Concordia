#include "graphics/userinterface/widgets/text.hpp"
#include "graphics/userinterface/propertylist.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {
      namespace Widgets {

        void Text::render( Device::Display::Adapter::Component::GuiComponent& manager ) {

        }

        void Text::calculate() {
          int padding = localStyle.get< int >( "padding" );
          int len = innerText.size();
          int fontSize = localStyle.get< int >( "font-size" );

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
