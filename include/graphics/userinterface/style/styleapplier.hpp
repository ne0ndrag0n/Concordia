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
          using CallResult = std::variant< int, double, std::string, bool, Gravity, Requisition, Placement, Orientation, glm::uvec4, LayoutProportions >;
          struct AppliedStyle {
            int specificity = -1;
            std::vector< AST::PropertyList > lists;
          };

          std::shared_ptr< Element > rootElement;
          std::vector< AST::PropertyList > propertyLists;

          CallResult call( const AST::Call& functionCall );
          std::variant< Gravity, Requisition, Placement, Orientation, int > identifier( const AST::Identifier& identifier );
          CallResult resolveValue( const std::variant< AST::Call, AST::Identifier, AST::Literal >& type );

          int getIntSetting( const std::string& key );
          glm::uvec4 rgbaString( const std::string& format );
          LayoutProportions createLayout( const std::vector< int >& proportions );
          glm::uvec4 getPlacement( int x, int y );
          int add( int first, int last );
          int subtract( int first, int last );
          int multiply( int first, int last );
          int divide( int first, int last );

          std::vector< AST::PropertyList > desugar( AST::PropertyList propertyList, std::vector< AST::SelectorQuery > parentQueries = {} );
          bool elementMatches( std::shared_ptr< Element > element, const AST::PropertyList& propertyList );

        public:
          EXCEPTION_TYPE( UndefinedSymbolException, "Symbol or function undefined" );
          EXCEPTION_TYPE( TypeMismatchException, "Type mismatch encountered" );
          EXCEPTION_TYPE( MalformedFormatException, "Malformed string format" );

          StyleApplier( std::shared_ptr< Element > rootElement );

          void update( std::shared_ptr< Element > element );
          void applyStyles( std::vector< std::string > paths );
          void applySnippet( const std::string& snippet );
        };

      }
    }
  }
}

#endif
