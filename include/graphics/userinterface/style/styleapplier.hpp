#ifndef STYLE_APPLIER
#define STYLE_APPLIER

#include "exceptions/genexc.hpp"
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
            int specificity = -1;
            std::vector< AST::PropertyList > lists;
          };

          std::unordered_map< std::shared_ptr< Element >, AppliedStyle > associations;
          std::shared_ptr< Element > rootElement;

          void paint();
          void applyLiteral( std::shared_ptr< Element > target, const std::string& key, const AST::Literal& literal );
          void applyIdentifier( std::shared_ptr< Element > target, const std::string& key, const AST::Identifier& identifier );
          void associatePropertyList( const AST::PropertyList& propertyList );
          std::vector< AST::PropertyList > desugar( AST::PropertyList propertyList, std::vector< AST::SelectorQuery > parentQueries = {} );
          bool elementMatchesQuery( const AST::SelectorQuery& query, std::shared_ptr< Element > element );

        public:
          EXCEPTION_TYPE( UndefinedSymbolException, "Symbol or function undefined" );

          StyleApplier( std::shared_ptr< Element > rootElement );

          void applyStyles( std::vector< std::string > paths );
        };

      }
    }
  }
}

#endif
