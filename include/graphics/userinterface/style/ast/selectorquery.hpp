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

            std::string generateSelectorString() const {
              std::string selectorResult;

              if( all ) {
                return "*";
              }

              if( tag.length() ) {
                selectorResult += tag;
              }

              if( id.length() ) {
                selectorResult += ( "#" + id );
              }

              for( std::string clss : classes ) {
                selectorResult += ( "." + clss );
              }

              return selectorResult;
            }

            unsigned int computeSpecificity() const {
              unsigned int specificity = 0;

              if( id.length() ) {
                specificity += 100;
              }

              specificity += 10 * classes.size();

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
