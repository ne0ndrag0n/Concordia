#include "graphics/userinterface/style/style.hpp"
#include "graphics/userinterface/element.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {
      namespace Style {

        Style::Style( Element* parent ) : parent( parent ) {}

        void Style::reflowParent() {
          parent->paint();
        }

        void Style::setCalculated( const std::unordered_map< std::string, std::any >& map ) {
          calculated = PropertyList( map );
        }

      }
    }
  }
}
