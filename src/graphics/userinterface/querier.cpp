#include "graphics/userinterface/querier.hpp"
#include "graphics/userinterface/element.hpp"
#include "graphics/userinterface/style/ast/selectorquery.hpp"
#include "tools/utility.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {

      Querier::Querier( std::shared_ptr< Element > element ) : rootElement( element ) {}

      bool Querier::matches( std::shared_ptr< Element > element, const Style::AST::SelectorQuery& selectorPart ) {
        if( selectorPart.all ) {
          return true;
        }

        if( selectorPart.tag.length() && element->getTag() != selectorPart.tag ) {
          return false;
        }

        if( selectorPart.id.length() && element->getId() != selectorPart.id ) {
          return false;
        }

        for( const std::string& clss : selectorPart.classes ) {
          if( !element->hasClass( clss ) ) {
            return false;
          }
        }

        return true;
      }

      /**
       * General search for all elements matching a SelectorQuery
       */
      std::vector< std::shared_ptr< Element > > Querier::getMatchingElements( std::shared_ptr< Element > current, const Style::AST::SelectorQuery& selectorPart ) {
        std::vector< std::shared_ptr< Element > > result;

        if( matches( current, selectorPart ) ) {
          result.push_back( current );
        }

        std::vector< std::shared_ptr< Element > > children = current->getChildren();
        for( std::shared_ptr< Element > child : children ) {
          result = Tools::Utility::concatArrays( result, getMatchingElements( child, selectorPart ) );
        }

        return result;
      }

      std::vector< std::shared_ptr< Element > > Querier::get( std::vector< Style::AST::SelectorQuery > selector ) {
        std::vector< std::shared_ptr< Element > > rightmost = getMatchingElements( rootElement, selector.back() );
        selector.pop_back();

        for( auto it = selector.rbegin(); it != selector.rend(); ++it ) {
          if( rightmost.empty() ) {
            break;
          }

          std::vector< std::shared_ptr< Element > > newRightmost;

          for( auto element : rightmost ) {
            std::shared_ptr< Element > parent = element->getParent();
            if( parent && matches( parent, *it ) ) {
              newRightmost.push_back( parent );
            }
          }

          rightmost = newRightmost;
        }

        return rightmost;
      }

    }
  }
}
