#include "graphics/userinterface/style/styleapplier.hpp"
#include "graphics/userinterface/style/parser.hpp"
#include "graphics/userinterface/element.hpp"
#include "tools/utility.hpp"
#include "log.hpp"
#include <fstream>
#include <stack>

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {
      namespace Style {

        StyleApplier::StyleApplier( std::shared_ptr< Element > rootElement ) : rootElement( rootElement ) {}

        void StyleApplier::associatePropertyList( const AST::PropertyList& propertyList ) {
          std::vector< AST::PropertyList > desugared = desugar( propertyList );

          for( const AST::PropertyList& desugar : desugared ) {
            desugar.print();
          }
        }

        /**
         * Given a set of tree-structured AST::PropertyList objects,
         */
        std::vector< AST::PropertyList > StyleApplier::desugar( AST::PropertyList propertyList, std::vector< AST::SelectorQuery > parentQueries ) {
          std::vector< AST::PropertyList > desugared;

          propertyList.selectorQueries = Tools::Utility::concatArrays( parentQueries, propertyList.selectorQueries );
          parentQueries = propertyList.selectorQueries;
          desugared.push_back( propertyList );

          for( AST::PropertyList& child : propertyList.children ) {
            desugared = Tools::Utility::concatArrays( desugared, desugar( child, parentQueries ) );
          }

          for( AST::PropertyList& child : desugared ) {
            child.children.clear();
          }

          return desugared;
        }

        bool StyleApplier::elementMatchesQuery( const AST::SelectorQuery& query, std::shared_ptr< Element > element ) {
          // Nothing else matters for the "all"/* selector
          if( query.all ) {
            return true;
          }

          if( query.tag.length() && element->getTag() != query.tag ) {
            return false;
          }

          if( query.id.length() && element->getId() != query.id ) {
            return false;
          }

          for( const std::string& clss : query.classes ) {
            if( !element->hasClass( clss ) ) {
              return false;
            }
          }

          // All other tests pass
          return true;
        }

        void StyleApplier::applyStyles( std::vector< std::string > paths ) {
          // * Load from file using an individual Style::Parser
          // * Find elements it applies to using specificity rules
          // * Write element rules (reflow should be the responsibility of the call that sets the style)

          for( const std::string& path : paths ) {
            std::vector< AST::PropertyList > stylesheet;

            try {
              Parser parser( path );
              stylesheet = parser.getStylesheet();
            } catch( std::exception e ) {
              Log::getInstance().warn( "StyleApplier::applyStyles", "Failed to load .style file: " + path + " (" + e.what() + ")" );
              continue;
            }

            for( const AST::PropertyList& propertyList : stylesheet ) {
              associatePropertyList( propertyList );
            }
          }
        }

      }
    }
  }
}

/**
std::vector< std::shared_ptr< Element > > StyleApplier::getElementsForQuery( std::vector< AST::SelectorQuery > selectorQueries ) {
  std::vector< std::shared_ptr< Element > > results = { rootElement };

  while( !selectorQueries.empty() ) {
    AST::SelectorQuery query = selectorQueries.front();
    selectorQueries.erase( selectorQueries.begin() );

    std::vector< std::shared_ptr< Element > > newResults;
    for( auto result : results ) {
      auto children = result->getChildren();
      for( auto child : children ) {
        if( elementMatchesQuery( query, child ) ) {
          newResults.push_back( child );
        }
      }
    }

    results = newResults;
  }

  return results;
}
*/
