#ifndef STYLE_APPLIER
#define STYLE_APPLIER

#include "graphics/userinterface/propertylist.hpp"
#include "graphics/userinterface/style/ast/propertylist.hpp"
#include <unordered_map>
#include <memory>
#include <vector>
#include <string>

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {
      class Element;

      namespace Style {
        namespace AST {
          class SelectorQuery;
        }

        class StyleApplier {
          struct AppliedStyle {
            unsigned int specificity;
            AST::PropertyList propertyList;
          };

          std::unordered_map< std::shared_ptr< Element >, std::vector< AppliedStyle > > propLists;
          std::shared_ptr< Element > rootElement;

          void associatePropertyList( const AST::PropertyList& propertyList );
          std::vector< AST::PropertyList > desugar( AST::PropertyList propertyList, std::vector< AST::SelectorQuery > parentQueries = {} );
          bool elementMatchesQuery( const AST::SelectorQuery& query, std::shared_ptr< Element > element );

        public:
          StyleApplier( std::shared_ptr< Element > rootElement );

          void applyStyles( std::vector< std::string > paths );
        };

      }
    }
  }
}

#endif
