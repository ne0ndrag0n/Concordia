#ifndef NEW_GUI_LINEAR_LAYOUT
#define NEW_GUI_LINEAR_LAYOUT

#include "graphics/userinterface/element.hpp"
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <memory>

namespace BlueBear {
  namespace Graphics {
    namespace Vector {
      class Renderer;
    }

    namespace UserInterface {
      namespace Widgets {

        class Layout : public Element {
        protected:
          Layout( const std::string& id, const std::vector< std::string >& classes );

          bool valueIsLiteral( int r );

        public:
          virtual void positionAndSizeChildren();
          virtual void render( Graphics::Vector::Renderer& renderer );
          virtual glm::uvec2 getRequestedSize();

          std::shared_ptr< Layout > create( const std::string& id, const std::vector< std::string >& classes );
        };

      }
    }
  }
}

#endif
