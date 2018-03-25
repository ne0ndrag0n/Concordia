#ifndef SELECTOR_QUERY
#define SELECTOR_QUERY

#include <string>
#include <vector>

#include "log.hpp"

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

            unsigned int computeSpecificity() const {
              unsigned int specificity = 0;

              if( id.length() ) {
                specificity += 100;
              }

              for( auto& clss : classes ) {
                specificity += 10;
              }

              if( tag.length() ) {
                specificity += 1;
              }

              return specificity;
            }

          };

        }
      }
    }
  }
}

#endif
