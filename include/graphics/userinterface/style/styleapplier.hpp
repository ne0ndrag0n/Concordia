#ifndef STYLE_APPLIER
#define STYLE_APPLIER

#include "graphics/userinterface/propertylist.hpp"
#include <unordered_map>
#include <memory>
#include <vector>
#include <string>

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {
      class Element;

      namespace Style {

        class StyleApplier {
          std::unordered_map< std::shared_ptr< Element >, std::vector< PropertyList > > propLists;
          std::shared_ptr< Element > rootElement;

        public:
          StyleApplier( std::shared_ptr< Element > rootElement );

          void applyStyles( std::vector< std::string > paths );
        };

      }
    }
  }
}

#endif
