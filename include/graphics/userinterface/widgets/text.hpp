#ifndef NEW_GUI_ELEMENT_TEXT
#define NEW_GUI_ELEMENT_TEXT

#include "graphics/userinterface/element.hpp"
#include <string>

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {
      namespace Widgets {

        class Text : public Element {
          std::string innerText;

        protected:
          Text( const std::string& id, const std::vector< std::string >& classes, const std::string& innerText );

        public:
          virtual void render( Device::Display::Adapter::Component::GuiComponent& manager ) override;
          virtual void calculate() override;

          void setText( const std::string& text );
          std::string getText();

          static std::shared_ptr< Text > create( const std::string& id, const std::vector< std::string >& classes, const std::string& innerText );
        };

      }
    }
  }
}

#endif
