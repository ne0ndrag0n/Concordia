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

        void StyleApplier::paint() {
          for( auto& pair : associations ) {
            for( const AST::PropertyList& propertyList : pair.second.lists ) {
              for( const AST::Property& property : propertyList.properties ) {
                if( std::holds_alternative< AST::Call >( property.value ) ) {
                  CallResult result = call( std::get< AST::Call >( property.value ) );

                  std::visit( [ & ]( auto& data ) {
                    pair.first->getPropertyList().set( property.name, data, false );
                  }, result );
                } else if( std::holds_alternative< AST::Literal >( property.value ) ) {
                  std::visit( [ & ]( auto& data ) {
                    pair.first->getPropertyList().set( property.name, data, false );
                  }, std::get< AST::Literal >( property.value ).data );
                } else if( std::holds_alternative< AST::Identifier >( property.value ) ) {
                  std::variant< Gravity, Requisition, Placement, Orientation > variant = identifier( std::get< AST::Identifier >( property.value ) );

                  std::visit( [ & ]( auto& data ) {
                    pair.first->getPropertyList().set( property.name, data, false );
                  }, variant );
                } else {
                  Log::getInstance().error( "StyleApplier::paint", "This should never happen" );
                }
              }

              if( propertyList.properties.size() ) {
                pair.first->getPropertyList().reflowParent();
              }
            }
          }
        }

        StyleApplier::CallResult StyleApplier::getArgument( const std::variant< AST::Call, AST::Identifier, AST::Literal >& type ) {
          CallResult argument;

          if( std::holds_alternative< AST::Call >( type ) ) {
            argument = call( std::get< AST::Call >( type ) );
          } else if( std::holds_alternative< AST::Identifier >( type ) ) {
            std::variant< Gravity, Requisition, Placement, Orientation > intermediate = identifier( std::get< AST::Identifier >( type ) );
            std::visit( [ & ]( auto& data ) {
              argument = data;
            }, intermediate );
          } else if( std::holds_alternative< AST::Literal >( type ) ) {
            std::visit( [ & ]( auto& data ) {
              argument = data;
            }, std::get< AST::Literal >( type ).data );
          } else {
            Log::getInstance().error( "StyleApplier::getArgument", "This should never happen" );
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
              CallResult argument = getArgument( functionCall.arguments.front() );

              if( std::holds_alternative< std::string >( argument ) ) {
                return getIntSetting( std::get< std::string >( argument ) );
              } else {
                throw TypeMismatchException();
              }
            }
            case Tools::Utility::hash( "rgbaString" ): {
              CallResult argument = getArgument( functionCall.arguments.front() );

              if( std::holds_alternative< std::string >( argument ) ) {
                return rgbaString( std::get< std::string >( argument ) );
              } else {
                throw TypeMismatchException();
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

        std::variant< Gravity, Requisition, Placement, Orientation > StyleApplier::identifier( const AST::Identifier& identifier ) {
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
                    return Requisition::AUTO;
                  }
                  case Tools::Utility::hash( "NONE" ): {
                    return Requisition::NONE;
                  }
                  case Tools::Utility::hash( "FILL_PARENT" ): {
                    return Requisition::FILL_PARENT;
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

          paint();
        }

      }
    }
  }
}
