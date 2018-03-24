#ifndef SE_QUERIER
#define SE_QUERIER

#include <unordered_map>
#include <memory>
#include <vector>

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {
      class Element;
      namespace Style {
        namespace AST {
          struct SelectorQuery;
        }
      }

      class Querier {
        std::shared_ptr< Element > rootElement;

        bool matches( std::shared_ptr< Element > element, const Style::AST::SelectorQuery& selectorPart );
        std::vector< std::shared_ptr< Element > > getMatchingElements(
          std::shared_ptr< Element > current,
          const Style::AST::SelectorQuery& selectorPart
        );

      public:
        Querier( std::shared_ptr< Element > element );
        std::vector< std::shared_ptr< Element > > get( std::vector< Style::AST::SelectorQuery > selector );
      };

    }
  }
}

#endif
