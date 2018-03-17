#include "graphics/userinterface/style/parser.hpp"
#include "log.hpp"
#include <any>

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {
      namespace Style {

        Parser::Parser( const std::string& path ) : path( path ), file( path ) {}

        bool Parser::checkAndAdvanceChar( char expect ) {
          char peek = ( char ) file.peek();
          if( peek == expect ) {
            file.get();
            currentColumn++;
            return true;
          }

          return false;
        }

        /**
         * static
         */
        bool Parser::isAlpha( char c ) {
          return ( c >= 'A' && c <= 'Z' ) || ( c >= 'a' && c <= 'z' ) || c == '_';
        }

        /**
         * static
         */
        bool Parser::isNumeric( char c ) {
          return ( c >= '0' && c <= '9' );
        }

        std::string Parser::getWhile( std::function< bool( char ) > predicate ) {
          std::string result;

          char c = ( char ) file.peek();
          while( predicate( c ) ) {
            result += c;
            c = file.get();
            currentColumn++;
          }

          return result;
        }

        void Parser::getTokens() {
          tokens = {};
          currentRow = 0;
          currentColumn = 0;

          char current;
          while( file.get( current ) ) {
            switch( current ) {
              case '#':
                tokens.push_back( { currentRow, currentColumn, TokenType::POUND } );
                currentColumn++;
                break;
              case '.':
                tokens.push_back( { currentRow, currentColumn, TokenType::DOT } );
                currentColumn++;
                break;
              case '*':
                tokens.push_back( { currentRow, currentColumn, TokenType::STAR } );
                currentColumn++;
                break;
              case '{':
                tokens.push_back( { currentRow, currentColumn, TokenType::LEFT_BRACE } );
                currentColumn++;
                break;
              case '}':
                tokens.push_back( { currentRow, currentColumn, TokenType::RIGHT_BRACE } );
                currentColumn++;
                break;
              case ';':
                tokens.push_back( { currentRow, currentColumn, TokenType::SEMICOLON } );
                currentColumn++;
                break;
              case ',':
                tokens.push_back( { currentRow, currentColumn, TokenType::COMMA } );
                currentColumn++;
                break;
              case ':':
                tokens.push_back( { currentRow, currentColumn, checkAndAdvanceChar( ':' ) ? TokenType::SCOPE_RESOLUTION : TokenType::COLON } );
                currentColumn++;
                break;
              case '(':
                tokens.push_back( { currentRow, currentColumn, TokenType::LEFT_PAREN } );
                currentColumn++;
                break;
              case ')':
                tokens.push_back( { currentRow, currentColumn, TokenType::RIGHT_PAREN } );
                currentColumn++;
                break;
              case ' ':
              case '\r':
              case '\t':
                currentColumn++;
              case '\n':
                currentColumn = 0;
                currentRow++;
                continue;
              case '"':
                // String literal
                tokens.push_back( {
                  currentRow, currentColumn,
                  TokenType::STRING,
                  getWhile( []( char c ) {
                    return !( c == '"' );
                  } )
                } );
                break;
              default: {
                if( isNumeric( current ) ) {
                  std::string num;
                  num += current;
                  num += getWhile( isNumeric );

                  if( checkAndAdvanceChar( '.' ) ) {
                    // Double literal
                    num += '.';
                    num += getWhile( isNumeric );

                    tokens.push_back( { currentRow, currentColumn, TokenType::DOUBLE, std::stod( num ) } );
                  } else {
                    // Integer literal
                    tokens.push_back( { currentRow, currentColumn, TokenType::INTEGER, std::stoi( num ) } );
                  }
                } else if( isAlpha( current ) ) {
                  std::string identifier;
                  identifier += current;
                  identifier += getWhile( isAlpha );

                  if( identifier == "true" || identifier == "false" ) {
                    // Boolean
                    tokens.push_back( { currentRow, currentColumn, TokenType::BOOLEAN, identifier == "true" } );
                  } else {
                    // Literal
                    tokens.push_back( { currentRow, currentColumn, TokenType::IDENTIFIER, identifier } );
                  }
                } else {
                  Log::getInstance().error(
                    "Parser::getTokens",
                    std::string( "While parsing " ) + path + ": Unexpected character at " + std::to_string( currentRow ) + ", " + std::to_string( currentColumn )
                  );
                  throw LexException();
                }
              }
            }
          }
        }

        void Parser::throwParseException( const std::string& expectation ) {
          if( tokens.empty() ) {
            Log::getInstance().error( "Parser::throwParseException",
              std::string( "Expected: " ) + expectation
            );
          } else {
            Log::getInstance().error( "Parser::throwParseException",
              std::string( "Expected: " ) + expectation + " near "
              + std::to_string( tokens.front().row ) + ", " + std::to_string( tokens.front().column )
            );
          }
        }

        bool Parser::checkToken( TokenType expectedType ) {
          return checkToken( tokens.begin(), expectedType );
        }

        bool Parser::checkToken( std::list< Token >::iterator iterator, TokenType expectedType ) {
          if( iterator == tokens.end() ) {
            return false;
          }

          return iterator->type == expectedType;
        }

        bool Parser::lookaheadAndCheckToken( std::list< Token >::iterator iterator, unsigned int lookahead, TokenType expectedType ) {
          for( unsigned int i = 0; i != lookahead; i++ ) {
            std::advance( iterator, 1 );

            if( iterator == tokens.end() ) {
              return false;
            }
          }

          return checkToken( iterator, expectedType );
        }

        Token Parser::getAndExpect( TokenType expectedType, const std::string& expectation ) {
          if( !checkToken( expectedType ) ) {
            throwParseException( expectation );
          }

          Token next = tokens.front();
          increment();
          return next;
        }

        void Parser::increment() {
          if( tokens.begin() != tokens.end() ) {
            tokens.pop_front();
          }
        }

        bool Parser::isSelectorToken() {
          return checkToken( TokenType::IDENTIFIER ) || checkToken( TokenType::POUND ) ||
            checkToken( TokenType::DOT ) || checkToken( TokenType::STAR );
        }

        AST::Call Parser::getCall() {
          AST::Call call;

          call.identifier = getIdentifier();

          getAndExpect( TokenType::LEFT_PAREN, "(" );

          while( true ) {
            if( checkToken( TokenType::INTEGER ) ) {
              call.arguments.push_back( AST::Literal{ std::any_cast< int >( tokens.front().metadata ) } );
              increment();
            } else if( checkToken( TokenType::DOUBLE ) ) {
              call.arguments.push_back( AST::Literal{ std::any_cast< double >( tokens.front().metadata ) } );
              increment();
            } else if( checkToken( TokenType::BOOLEAN ) ) {
              call.arguments.push_back( AST::Literal{ std::any_cast< bool >( tokens.front().metadata ) } );
              increment();
            } else if( checkToken( TokenType::STRING ) ) {
              call.arguments.push_back( AST::Literal{ std::any_cast< std::string >( tokens.front().metadata ) } );
              increment();
            } else if( checkToken( TokenType::IDENTIFIER ) ) {
              auto current = tokens.begin();
              while( true ) {
                if( !checkToken( current, TokenType::IDENTIFIER ) ) {
                  break;
                }

                std::advance( current, 1 );

                if( !checkToken( current, TokenType::SCOPE_RESOLUTION ) ) {
                  break;
                }

                std::advance( current, 1 );
              }

              if( checkToken( current, TokenType::LEFT_PAREN ) ) {
                call.arguments.push_back( getCall() );
              } else {
                call.arguments.push_back( getIdentifier() );
              }
            } else {
              throwParseException( "integer, double, boolean, string, or identifier token" );
            }

            if( checkToken( TokenType::COMMA ) ) {
              increment();
            } else {
              break;
            }
          }

          getAndExpect( TokenType::RIGHT_PAREN, ")" );

          return call;
        }

        AST::Identifier Parser::getIdentifier() {
          AST::Identifier identifier;

          while( true ) {
            // Lookahead for SCOPE_RESOLUTION
            if( lookaheadAndCheckToken( tokens.begin(), 1, TokenType::SCOPE_RESOLUTION ) ) {
              Token identifierToken = getAndExpect( TokenType::IDENTIFIER, "identifier" );
              identifier.scope.push_back( std::any_cast< std::string >( identifierToken.metadata ) );

              // Pop the SCOPE_RESOLUTION token
              increment();
            } else {
              break;
            }
          }

          Token identToken = getAndExpect( TokenType::IDENTIFIER, "identifier" );
          identifier.value = std::any_cast< std::string >( identToken.metadata );

          return identifier;
        }

        AST::SelectorQuery Parser::getSelectorQuery() {
          AST::SelectorQuery selectorQuery;

          if( checkToken( TokenType::STAR ) ) {
            selectorQuery.all = true;
            return selectorQuery;
          }

          if( checkToken( TokenType::IDENTIFIER ) ) {
            selectorQuery.tag = std::any_cast< std::string >( tokens.front().metadata );
            increment();
          }

          if( checkToken( TokenType::POUND ) ) {
            increment();
            selectorQuery.id = std::any_cast< std::string >( getAndExpect( TokenType::IDENTIFIER, "identifier" ) );
          }

          while( checkToken( TokenType::DOT ) ) {
            increment();
            selectorQuery.classes.push_back(
              std::any_cast< std::string >( getAndExpect( TokenType::IDENTIFIER, "identifier" ) )
            );
          }

          return selectorQuery;
        }

        AST::Property Parser::getProperty() {
          AST::Property property;

          property.name = std::any_cast< std::string >( tokens.front().metadata );
          increment();

          getAndExpect( TokenType::COLON, ":" );

          if( checkToken( TokenType::INTEGER ) ) {
            property.value = AST::Literal{ std::any_cast< int >( tokens.front().metadata ) };
            increment();
          } else if( checkToken( TokenType::DOUBLE ) ) {
            property.value = AST::Literal{ std::any_cast< double >( tokens.front().metadata ) };
            increment();
          } else if( checkToken( TokenType::BOOLEAN ) ) {
            property.value = AST::Literal{ std::any_cast< bool >( tokens.front().metadata ) };
            increment();
          } else if( checkToken( TokenType::STRING ) ) {
            property.value = AST::Literal{ std::any_cast< std::string >( tokens.front().metadata ) };
            increment();
          } else if( checkToken( TokenType::IDENTIFIER ) ) {
            auto current = tokens.begin();
            while( true ) {
              if( !checkToken( current, TokenType::IDENTIFIER ) ) {
                break;
              }

              std::advance( current, 1 );

              if( !checkToken( current, TokenType::SCOPE_RESOLUTION ) ) {
                break;
              }

              std::advance( current, 1 );
            }

            if( checkToken( current, TokenType::LEFT_PAREN ) ) {
              property.value = getCall();
            } else {
              property.value = getIdentifier();
            }
          } else {
            throwParseException( "integer, double, boolean, string, or identifier token" );
          }

          getAndExpect( TokenType::SEMICOLON, ";" );

          return property;
        }

        AST::PropertyList Parser::getPropertyList() {
          AST::PropertyList rootPropertyList;

          // Get the selector
          if( !isSelectorToken() ) {
            throwParseException( "identifier, #, ., or *" );
          }

          while( isSelectorToken() ) {
            rootPropertyList.selectorQueries.push_back( getSelectorQuery() );
          }

          getAndExpect( TokenType::LEFT_BRACE, "{" );

          while( checkToken( TokenType::IDENTIFIER ) || isSelectorToken() ) {
            auto nextAfter = tokens.begin();
            std::advance( nextAfter, 1 );

            if( checkToken( nextAfter, TokenType::COLON ) ) {
              // Identifier plus colon is a property token
              rootPropertyList.properties.push_back( getProperty() );
            } else {
              // It can only be another property list
              rootPropertyList.children.push_back( getPropertyList() );
            }
          }

          getAndExpect( TokenType::RIGHT_BRACE, "}" );

          return rootPropertyList;
        }

        std::vector< AST::PropertyList > Parser::getStylesheet() {
          std::vector< AST::PropertyList > stylesheet;

          while( isSelectorToken() ) {
            stylesheet.emplace_back( getPropertyList() );
          }

          return stylesheet;
        }

      }
    }
  }
}
