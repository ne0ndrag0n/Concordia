#include "graphics/userinterface/element.hpp"
#include "graphics/vector/renderer.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {

      Element::Element( const std::string& tag, const std::string& id, const std::vector< std::string >& classes ) : tag( tag ), id( id ), classes( classes ) {}

      std::shared_ptr< Element > Element::create( const std::string& tag, const std::string& id, const std::vector< std::string >& classes ) {
        std::shared_ptr< Element > result( new Element( tag, id, classes ) );

        return result;
      }

      /**
       * Reflow element and all children (rerender quad and texture as well as all its children)
       */
      void Element::reflow( Vector::Renderer& vectorRenderer ) {
        
      }

    }
  }
}
