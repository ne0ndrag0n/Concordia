#ifndef STYLE_APPLIER
#define STYLE_APPLIER

#include "exceptions/genexc.hpp"
#include "graphics/userinterface/propertylist.hpp"
#include "graphics/userinterface/style/ast/propertylist.hpp"
#include <glm/glm.hpp>
#include <unordered_map>
#include <memory>
#include <vector>
#include <string>
#include <variant>

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {
      class Element;

      namespace Style {
        namespace AST {
          class SelectorQuery;
        }

        class StyleApplier {
          using CallResult = std::variant< int, double, std::string, bool, Gravity, Requisition, Placement, Orientation, glm::uvec4 >;
          struct AppliedStyle {
            int specificity = -1;
            std::vector< AST::PropertyList > lists;
          };

          std::unordered_map< std::shared_ptr< Element >, AppliedStyle > associations;
          std::shared_ptr< Element > rootElement;

          void paint();

          CallResult call( const AST::Call& functionCall );
          std::variant< Gravity, Requisition, Placement, Orientation > identifier( const AST::Identifier& identifier );
          CallResult getArgument( const std::variant< AST::Call, AST::Identifier, AST::Literal >& type );

          int getIntSetting( const std::string& key );
          glm::uvec4 rgbaString( const std::string& format );

          void associatePropertyList( const AST::PropertyList& propertyList );
          std::vector< AST::PropertyList > desugar( AST::PropertyList propertyList, std::vector< AST::SelectorQuery > parentQueries = {} );

        public:
          EXCEPTION_TYPE( UndefinedSymbolException, "Symbol or function undefined" );
          EXCEPTION_TYPE( TypeMismatchException, "Type mismatch encountered" );
          EXCEPTION_TYPE( MalformedFormatException, "Malformed string format" );

          StyleApplier( std::shared_ptr< Element > rootElement );

          void applyStyles( std::vector< std::string > paths );
        };

      }
    }
  }
}

#endif
