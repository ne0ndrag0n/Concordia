#include "graphics/userinterface/style/styleapplier.hpp"
#include "graphics/userinterface/style/parser.hpp"
#include "graphics/userinterface/element.hpp"
#include "graphics/userinterface/querier.hpp"
#include "tools/utility.hpp"
#include "log.hpp"
#include <fstream>
#include <stack>

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {
      namespace Style {

        StyleApplier::StyleApplier( std::shared_ptr< Element > rootElement ) : rootElement( rootElement ) {}

        void StyleApplier::paint() {
          for( auto& pair : associations ) {
            for( const AST::PropertyList& propertyList : pair.second.lists ) {
              for( const AST::Property& property : propertyList.properties ) {
                if( std::holds_alternative< AST::Call >( property.value ) ) {

                } else if( std::holds_alternative< AST::Literal >( property.value ) ) {
                  applyLiteral( pair.first, property.name, std::get< AST::Literal >( property.value ) );
                } else if( std::holds_alternative< AST::Identifier >( property.value ) ) {
                  applyIdentifier( pair.first, property.name, std::get< AST::Identifier >( property.value ) );
                } else {
                  Log::getInstance().error( "StyleApplier::paint", "This should never happen" );
                }
              }
            }
          }
        }

        void StyleApplier::applyLiteral( std::shared_ptr< Element > target, const std::string& key, const AST::Literal& literal ) {
          if( std::holds_alternative< int >( literal.data ) ) {
            target->getPropertyList().set( key, std::get< int >( literal.data ), false );
          } else if( std::holds_alternative< double >( literal.data ) ) {
            target->getPropertyList().set( key, std::get< double >( literal.data ), false );
          } else if( std::holds_alternative< std::string >( literal.data ) ) {
            target->getPropertyList().set( key, std::get< std::string >( literal.data ), false );
          } else if( std::holds_alternative< bool >( literal.data ) ) {
            target->getPropertyList().set( key, std::get< bool >( literal.data ), false );
          } else {
            Log::getInstance().error( "StyleApplier::applyLiteral", "This should never happen" );
          }
        }

        void StyleApplier::applyIdentifier( std::shared_ptr< Element > target, const std::string& key, const AST::Identifier& identifier ) {
          if( identifier.scope.size() == 1 ) {
            std::string single = identifier.scope.front();
            switch( Tools::Utility::hash( single.c_str() ) ) {
              case Tools::Utility::hash( "Gravity" ): {
                switch( Tools::Utility::hash( identifier.value.c_str() ) ) {
                  case Tools::Utility::hash( "LEFT" ): {
                    target->getPropertyList().set( key, Gravity::LEFT, false );
                    break;
                  }
                  case Tools::Utility::hash( "RIGHT" ): {
                    target->getPropertyList().set( key, Gravity::RIGHT, false );
                    break;
                  }
                  case Tools::Utility::hash( "TOP" ): {
                    target->getPropertyList().set( key, Gravity::TOP, false );
                    break;
                  }
                  case Tools::Utility::hash( "BOTTOM" ): {
                    target->getPropertyList().set( key, Gravity::BOTTOM, false );
                    break;
                  }
                  default:
                    throw UndefinedSymbolException();
                }
                return;
              }
              case Tools::Utility::hash( "Requisition" ): {
                switch( Tools::Utility::hash( identifier.value.c_str() ) ) {
                  case Tools::Utility::hash( "AUTO" ): {
                    target->getPropertyList().set( key, Requisition::AUTO, false );
                    break;
                  }
                  case Tools::Utility::hash( "NONE" ): {
                    target->getPropertyList().set( key, Requisition::NONE, false );
                    break;
                  }
                  case Tools::Utility::hash( "FILL_PARENT" ): {
                    target->getPropertyList().set( key, Requisition::FILL_PARENT, false );
                    break;
                  }
                  default:
                    throw UndefinedSymbolException();
                }
                return;
              }
              case Tools::Utility::hash( "Placement" ): {
                switch( Tools::Utility::hash( identifier.value.c_str() ) ) {
                  case Tools::Utility::hash( "FLOW" ): {
                    target->getPropertyList().set( key, Placement::FLOW, false );
                    break;
                  }
                  case Tools::Utility::hash( "FREE" ): {
                    target->getPropertyList().set( key, Placement::FREE, false );
                    break;
                  }
                  default:
                    throw UndefinedSymbolException();
                }
                return;
              }
              case Tools::Utility::hash( "Orientation" ): {
                switch( Tools::Utility::hash( identifier.value.c_str() ) ) {
                  case Tools::Utility::hash( "TOP" ): {
                    target->getPropertyList().set( key, Orientation::TOP, false );
                    break;
                  }
                  case Tools::Utility::hash( "MIDDLE" ): {
                    target->getPropertyList().set( key, Orientation::MIDDLE, false );
                    break;
                  }
                  case Tools::Utility::hash( "BOTTOM" ): {
                    target->getPropertyList().set( key, Orientation::BOTTOM, false );
                    break;
                  }
                  case Tools::Utility::hash( "LEFT" ): {
                    target->getPropertyList().set( key, Orientation::LEFT, false );
                    break;
                  }
                  case Tools::Utility::hash( "RIGHT" ): {
                    target->getPropertyList().set( key, Orientation::RIGHT, false );
                    break;
                  }
                  default:
                    throw UndefinedSymbolException();
                }
                return;
              }
              default:
                throw UndefinedSymbolException();
            }
          }

          throw UndefinedSymbolException();
        }

        void StyleApplier::associatePropertyList( const AST::PropertyList& propertyList ) {
          std::vector< AST::PropertyList > desugared = desugar( propertyList );

          for( const AST::PropertyList& list : desugared ) {
            Querier querier( rootElement );
            std::vector< std::shared_ptr< Element > > matches = querier.get( list.selectorQueries );
            unsigned int selectorSpecificity = list.computeSpecificity();

            Log::getInstance().debug( "StyleApplier::associatePropertyList", "Finding elements for " + list.generateSelectorString() );

            for( std::shared_ptr< Element > element : matches ) {
              if( selectorSpecificity == associations[ element ].specificity ) {
                associations[ element ].lists.push_back( list );
              } else if( selectorSpecificity > associations[ element ].specificity ) {
                associations[ element ] = {
                  ( int ) selectorSpecificity,
                  { list }
                };
              } else {
                Log::getInstance().warn( "StyleApplier::associatePropertyList", "Selector " + list.generateSelectorString() + " was not applied as it is not specific enough relative to other property lists." );
              }
            }
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
