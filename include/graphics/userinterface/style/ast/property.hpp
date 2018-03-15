#ifndef PARSER_PROPERTY
#define PARSER_PROPERTY

#include "graphics/userinterface/style/ast/call.hpp"
#include "graphics/userinterface/style/ast/literal.hpp"
#include "graphics/userinterface/style/ast/identifier.hpp"
#include <string>
#include <variant>

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {
      namespace Style {
        namespace AST {

          struct Property {
            std::string name;
            std::variant< Call, Literal, Identifier > value;
          };

        }
      }
    }
  }
}

#endif
