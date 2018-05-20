#ifndef NEW_GUI_ELEMENT_TEXT
#define NEW_GUI_ELEMENT_TEXT

#include "graphics/userinterface/element.hpp"
#include <string>
#include <functional>
#include <glm/glm.hpp>

namespace BlueBear {
  namespace Graphics {
    namespace Vector {
      class Renderer;
    }

    namespace UserInterface {
      namespace Widgets {

        class Text : public Element {
          std::string innerText;
          double textSpan = 0;

        protected:
          Text( const std::string& id, const std::vector< std::string >& classes, const std::string& innerText );

        public:
          virtual void render( Graphics::Vector::Renderer& renderer ) override;
          virtual void calculate() override;
          virtual void reflow( bool selectorsInvalidated = true ) override;

          void setText( const std::string& text, bool doReflow = true );
          std::string getText();

          static std::shared_ptr< Text > create( const std::string& id, const std::vector< std::string >& classes, const std::string& innerText );
        };

      }
    }
  }
}

#endif
