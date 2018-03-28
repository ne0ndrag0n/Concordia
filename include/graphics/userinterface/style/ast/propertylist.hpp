#ifndef PARSER_PROPERTY_LIST
#define PARSER_PROPERTY_LIST

#include "graphics/userinterface/style/ast/property.hpp"
#include "graphics/userinterface/style/ast/selectorquery.hpp"
#include <string>
#include <variant>

#include "log.hpp"
#include "tools/utility.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {
      namespace Style {
        namespace AST {

          struct PropertyList {
            std::vector< SelectorQuery > selectorQueries;
            std::vector< Property > properties;
            std::vector< PropertyList > children;

            unsigned int computeSpecificity() const {
              unsigned int totalSpecificity;

              for( const AST::SelectorQuery& selectorQuery : selectorQueries ) {
                totalSpecificity += selectorQuery.computeSpecificity();
              }

              return totalSpecificity;
            }

            std::string generateSelectorString( unsigned int indentation = 0 ) const {
              std::string selectorResult = Tools::Utility::generateIndentation( indentation );

              for( auto& selectorQuery : selectorQueries ) {
                selectorResult += ( selectorQuery.generateSelectorString() + " " );
              }

              return selectorResult;
            }

            void print( unsigned int indentation = 0 ) const {
              std::string selectorResult = generateSelectorString( indentation );

              Log::getInstance().debug( "PropertyList::print", selectorResult );

              for( auto& property : properties ) {
                property.print( indentation );
              }

              for( auto& child : children ) {
                child.print( indentation + 1 );
              }
            };
          };

        }
      }
    }
  }
}

#endif
