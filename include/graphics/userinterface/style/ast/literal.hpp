#ifndef PARSER_LITERAL
#define PARSER_LITERAL

#include <variant>
#include <string>

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {
      namespace Style {
        namespace AST {

          struct Literal {
            std::variant< int, double, std::string, bool > data;
          };

        }
      }
    }
  }
}

#endif
