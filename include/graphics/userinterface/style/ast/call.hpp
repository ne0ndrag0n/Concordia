#ifndef PARSER_CALL
#define PARSER_CALL

#include "graphics/userinterface/style/ast/identifier.hpp"
#include "graphics/userinterface/style/ast/literal.hpp"
#include <vector>
#include <string>

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {
      namespace Style {
        namespace AST {

          struct Call {
            Identifier identifier;
            std::vector< std::variant< Call, Identifier, Literal > > arguments;
          };

        }
      }
    }
  }
}

#endif
