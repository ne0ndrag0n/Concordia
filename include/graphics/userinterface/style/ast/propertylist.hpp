#ifndef PARSER_PROPERTY_LIST
#define PARSER_PROPERTY_LIST

#include "graphics/userinterface/style/ast/property.hpp"
#include "graphics/userinterface/style/ast/selectorquery.hpp"
#include <string>
#include <variant>

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {
      namespace Style {
        namespace AST {

          struct PropertyList {
            std::vector< SelectorQuery > selectorQueries;
            std::vector< Property > properties;
            std::vector< PropertyList > children;
          };

        }
      }
    }
  }
}

#endif
