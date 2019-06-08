#include "graphics/userinterface/style/styleapplier.hpp"
#include "graphics/userinterface/style/parser.hpp"
#include "graphics/userinterface/element.hpp"
#include "graphics/userinterface/querier.hpp"
#include "tools/utility.hpp"
#include "configmanager.hpp"
#include "log.hpp"
#include <fstream>
#include <sstream>
#include <stack>

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {
      namespace Style {

        StyleApplier::StyleApplier( std::shared_ptr< Element > rootElement ) : rootElement( rootElement ) {}

        StyleApplier::CallResult StyleApplier::resolveValue( const std::variant< AST::Call, AST::Identifier, AST::Literal >& type ) {
          CallResult argument;

          if( std::holds_alternative< AST::Call >( type ) ) {
            argument = call( std::get< AST::Call >( type ) );
          } else if( std::holds_alternative< AST::Identifier >( type ) ) {
            std::variant< Gravity, Requisition, Placement, Orientation, int > intermediate = identifier( std::get< AST::Identifier >( type ) );
            std::visit( [ & ]( auto& data ) {
              argument = data;
            }, intermediate );
          } else if( std::holds_alternative< AST::Literal >( type ) ) {
            std::visit( [ & ]( auto& data ) {
              argument = data;
            }, std::get< AST::Literal >( type ).data );
          } else {
            Log::getInstance().error( "StyleApplier::resolveValue", "This should never happen" );
          }

          return argument;
        }

        StyleApplier::CallResult StyleApplier::call( const AST::Call& functionCall ) {
          CallResult result;

          if( functionCall.identifier.scope.size() > 0 ) {
            throw UndefinedSymbolException();
          }

          switch( Tools::Utility::hash( functionCall.identifier.value.c_str() ) ) {
            case Tools::Utility::hash( "getIntSetting" ): {
              CallResult argument = resolveValue( functionCall.arguments.front() );

              if( std::holds_alternative< std::string >( argument ) ) {
                return getIntSetting( std::get< std::string >( argument ) );
              } else {
                throw TypeMismatchException();
              }
            }
            case Tools::Utility::hash( "rgbaString" ): {
              CallResult argument = resolveValue( functionCall.arguments.front() );

              if( std::holds_alternative< std::string >( argument ) ) {
                return rgbaString( std::get< std::string >( argument ) );
              } else {
                throw TypeMismatchException();
              }
            }
            case Tools::Utility::hash( "add" ): {
              CallResult argument1 = resolveValue( functionCall.arguments.at( 0 ) );
              CallResult argument2 = resolveValue( functionCall.arguments.at( 1 ) );

              if( std::holds_alternative< int >( argument1 ) && std::holds_alternative< int >( argument2 ) ) {
                return add( std::get< int >( argument1 ), std::get< int >( argument2 ) );
              }
            }
            case Tools::Utility::hash( "subtract" ): {
              CallResult argument1 = resolveValue( functionCall.arguments.at( 0 ) );
              CallResult argument2 = resolveValue( functionCall.arguments.at( 1 ) );

              if( std::holds_alternative< int >( argument1 ) && std::holds_alternative< int >( argument2 ) ) {
                return subtract( std::get< int >( argument1 ), std::get< int >( argument2 ) );
              }
            }
            case Tools::Utility::hash( "multiply" ): {
              CallResult argument1 = resolveValue( functionCall.arguments.at( 0 ) );
              CallResult argument2 = resolveValue( functionCall.arguments.at( 1 ) );

              if( std::holds_alternative< int >( argument1 ) && std::holds_alternative< int >( argument2 ) ) {
                return multiply( std::get< int >( argument1 ), std::get< int >( argument2 ) );
              }
            }
            case Tools::Utility::hash( "divide" ): {
              CallResult argument1 = resolveValue( functionCall.arguments.at( 0 ) );
              CallResult argument2 = resolveValue( functionCall.arguments.at( 1 ) );

              if( std::holds_alternative< int >( argument1 ) && std::holds_alternative< int >( argument2 ) ) {
                return divide( std::get< int >( argument1 ), std::get< int >( argument2 ) );
              }
            }
            default:
              throw UndefinedSymbolException();
          }
        }

        int StyleApplier::getIntSetting( const std::string& key ) {
          return ConfigManager::getInstance().getIntValue( key );
        }

        glm::uvec4 StyleApplier::rgbaString( const std::string& format ) {
          glm::uvec4 result;

          unsigned int len = format.size();
          unsigned int index = 0;
          for( unsigned int i = 0; i != len; i += 2 ) {
            std::istringstream stream( format.substr( i, 2 ) );
            unsigned int segment;
            stream >> std::hex >> segment;
            result[ index++ ] = segment;
          }

          return result;
        }

        int StyleApplier::add( int first, int last ) {
          return first + last;
        }

        int StyleApplier::subtract( int first, int last ) {
          return first - last;
        }

        int StyleApplier::multiply( int first, int last ) {
          return first * last;
        }

        int StyleApplier::divide( int first, int last ) {
          return first / last;
        }

        std::variant< Gravity, Requisition, Placement, Orientation, int > StyleApplier::identifier( const AST::Identifier& identifier ) {
          if( identifier.scope.size() == 1 ) {
            std::string single = identifier.scope.front();

            switch( Tools::Utility::hash( single.c_str() ) ) {
              case Tools::Utility::hash( "Gravity" ): {
                switch( Tools::Utility::hash( identifier.value.c_str() ) ) {
                  case Tools::Utility::hash( "LEFT" ): {
                    return Gravity::LEFT;
                  }
                  case Tools::Utility::hash( "RIGHT" ): {
                    return Gravity::RIGHT;
                  }
                  case Tools::Utility::hash( "TOP" ): {
                    return Gravity::TOP;
                  }
                  case Tools::Utility::hash( "BOTTOM" ): {
                    return Gravity::BOTTOM;
                  }
                  default:
                    throw UndefinedSymbolException();
                }
              }
              case Tools::Utility::hash( "Requisition" ): {
                switch( Tools::Utility::hash( identifier.value.c_str() ) ) {
                  case Tools::Utility::hash( "AUTO" ): {
                    return ( int ) Requisition::AUTO;
                  }
                  case Tools::Utility::hash( "NONE" ): {
                    return ( int ) Requisition::NONE;
                  }
                  case Tools::Utility::hash( "FILL_PARENT" ): {
                    return ( int ) Requisition::FILL_PARENT;
                  }
                  default:
                    throw UndefinedSymbolException();
                }
              }
              case Tools::Utility::hash( "Placement" ): {
                switch( Tools::Utility::hash( identifier.value.c_str() ) ) {
                  case Tools::Utility::hash( "FLOW" ): {
                    return Placement::FLOW;
                  }
                  case Tools::Utility::hash( "FREE" ): {
                    return Placement::FREE;
                  }
                  default:
                    throw UndefinedSymbolException();
                }
              }
              case Tools::Utility::hash( "Orientation" ): {
                switch( Tools::Utility::hash( identifier.value.c_str() ) ) {
                  case Tools::Utility::hash( "TOP" ): {
                    return Orientation::TOP;
                  }
                  case Tools::Utility::hash( "MIDDLE" ): {
                    return Orientation::MIDDLE;
                  }
                  case Tools::Utility::hash( "BOTTOM" ): {
                    return Orientation::BOTTOM;
                  }
                  case Tools::Utility::hash( "LEFT" ): {
                    return Orientation::LEFT;
                  }
                  case Tools::Utility::hash( "RIGHT" ): {
                    return Orientation::RIGHT;
                  }
                  default:
                    throw UndefinedSymbolException();
                }
              }
              default:
                throw UndefinedSymbolException();
            }
          }

          throw UndefinedSymbolException();
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

        bool StyleApplier::elementMatches( std::shared_ptr< Element > element, const AST::PropertyList& propertyList ) {
          for( auto it = propertyList.selectorQueries.rbegin(); it != propertyList.selectorQueries.rend(); ++it ) {
            if( !element || !Querier::matches( element, *it ) ) {
              return false;
            }

            auto next = it; std::advance( next, 1 );
            if( next != propertyList.selectorQueries.rend() ) {
              // Element must have at least one parent in the chain who matches the "next" query part
              element = element->getParent();
              while( element ) {
                if( Querier::matches( element, *next ) ) {
                  // Stop looking, we found it
                  break;
                }

                element = element->getParent();
              }
            }
          }

          return true;
        }

        void StyleApplier::update( std::shared_ptr< Element > element ) {
          // Get a list of propertylists that apply to this element
          std::vector< AST::PropertyList > applicablePropertyLists;
          for( const AST::PropertyList& list : propertyLists ) {
            if( elementMatches( element, list ) ) {
              applicablePropertyLists.push_back( list );
            }
          }

          if( applicablePropertyLists.size() ) {
            // Sort by specificity most to least
            std::stable_sort( applicablePropertyLists.begin(), applicablePropertyLists.end(), []( const AST::PropertyList& left, const AST::PropertyList& right ) {
              return left.computeSpecificity() > right.computeSpecificity();
            } );

            // Apply by increasing specificity
            // Unoptimised, naive implementation
            std::unordered_map< std::string, PropertyListType > values;
            int previousSpecificity = applicablePropertyLists.front().computeSpecificity();

            for( const AST::PropertyList& list : applicablePropertyLists ) {
              if( list.computeSpecificity() == previousSpecificity ) {
                // Indiscriminately apply properties
                for( const AST::Property& property : list.properties ) {
                  CallResult value = resolveValue( property.value );
                  std::visit( [ & ]( auto& data ) { values[ property.name ] = data; }, value );
                }
              } else {
                // Do not apply properties that are already applied
                for( const AST::Property& property : list.properties ) {
                  if( values.find( property.name ) == values.end() ) {
                    CallResult value = resolveValue( property.value );
                    std::visit( [ & ]( auto& data ) { values[ property.name ] = data; }, value );
                  }
                }
              }

              previousSpecificity = list.computeSpecificity();
            }

            element->getPropertyList().setCalculated( values );
          }

          // Recurse
          std::vector< std::shared_ptr< Element > > children = element->getChildren();
          for( std::shared_ptr< Element > child : children ) {
            update( child );
          }
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
              propertyLists = Tools::Utility::concatArrays( propertyLists, desugar( propertyList ) );
            }
          }

          // Everything needs to be redone
          rootElement->reflow();
        }

      }
    }
  }
}
