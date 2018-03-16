#ifndef SELECTOR_QUERY
#define SELECTOR_QUERY

#include <string>
#include <vector>

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {
      namespace Style {
        namespace AST {

          struct SelectorQuery {
            std::string tag;
            std::string id;
            std::vector< std::string > classes;
            bool all = false;
          };

        }
      }
    }
  }
}

#endif
