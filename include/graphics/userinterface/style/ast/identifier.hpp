#ifndef PARSER_CONSTANT
#define PARSER_CONSTANT

#include <vector>
#include <string>

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {
      namespace Style {
        namespace AST {

          struct Identifier {
            std::vector< std::string > scope;
            std::string value;
          };

        }
      }
    }
  }
}

#endif
