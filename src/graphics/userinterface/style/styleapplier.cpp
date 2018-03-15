#include "graphics/userinterface/style/styleapplier.hpp"
#include <fstream>

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {
      namespace Style {

        StyleApplier::StyleApplier( std::shared_ptr< Element > rootElement ) : rootElement( rootElement ) {}

        void StyleApplier::applyStyles( std::vector< std::string > paths ) {
          // Use Style::Parser to parse files into property list trees
          // Apply these trees to the appropriate elements starting with rootElement
        }

      }
    }
  }
}
