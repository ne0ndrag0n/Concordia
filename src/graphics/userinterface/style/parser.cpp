#include "graphics/userinterface/style/parser.hpp"
#include "log.hpp"
#include <any>

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {
      namespace Style {

        Parser::Parser( const std::string& path ) : path( path ) {
          file.exceptions( std::ios::failbit | std::ios::badbit );
          file.open( path );
          file.exceptions( std::ios::goodbit );

          getTokens();
        }

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
          return ( c >= 'A' && c <= 'Z' ) || ( c >= 'a' && c <= 'z' ) || c == '_' || c == '-';
        }

        /**
         * static
         */
        bool Parser::isNumeric( char c ) {
          return ( c >= '0' && c <= '9' );
        }

        std::string Parser::getWhile( std::function< bool( char ) > predicate ) {
          std::string result;

          while( true ) {
            char c = ( char ) file.peek();
            if( predicate( c ) ) {
              file.get();
              currentColumn++;
              result += c;
            } else {
              break;
            }
          }

          return result;
        }

        void Parser::getTokens() {
          tokens = {};
          currentRow = 1;
          currentColumn = 1;

          char current;
          bool negative = false;
          while( file.get( current ) ) {
            switch( current ) {
              case '#':
                tokens.push_back( { currentRow, currentColumn, TokenType::POUND } );
                break;
              case '.':
                tokens.push_back( { currentRow, currentColumn, TokenType::DOT } );
                break;
              case '*':
                tokens.push_back( { currentRow, currentColumn, TokenType::STAR } );
                break;
              case '{':
                tokens.push_back( { currentRow, currentColumn, TokenType::LEFT_BRACE } );
                break;
              case '}':
                tokens.push_back( { currentRow, currentColumn, TokenType::RIGHT_BRACE } );
                break;
              case ';':
                tokens.push_back( { currentRow, currentColumn, TokenType::SEMICOLON } );
                break;
              case ',':
                tokens.push_back( { currentRow, currentColumn, TokenType::COMMA } );
                break;
              case ':':
                tokens.push_back( { currentRow, currentColumn, checkAndAdvanceChar( ':' ) ? TokenType::SCOPE_RESOLUTION : TokenType::COLON } );
                break;
              case '(':
                tokens.push_back( { currentRow, currentColumn, TokenType::LEFT_PAREN } );
                break;
              case ')':
                tokens.push_back( { currentRow, currentColumn, TokenType::RIGHT_PAREN } );
                break;
              case ' ':
              case '\r':
              case '\t':
                break;
              case '\n':
                currentColumn = 0;
                currentRow++;
                break;
              case '"': {
                // String literal
                std::string literal = getWhile( []( char c ) {
                  return !( c == '"' );
                } );

                // Next token was peeked as a " so swallow it and increment the counter
                file.get();
                currentColumn++;

                tokens.push_back( { currentRow, currentColumn, TokenType::STRING, literal } );
                break;
              }
              case '-': {
                if( isNumeric( ( char ) file.peek() ) ) {
                  currentColumn++;
                  negative = true;
                  file.get( current );
                  // Fall through to number case; we know it's the next one up.
                } // else, we fall through to the isAlpha case
              }
              default: {
                if( isNumeric( current ) ) {
                  std::string num;
                  num += current;
                  num += getWhile( isNumeric );

                  if( checkAndAdvanceChar( '.' ) ) {
                    // Double literal
                    num += '.';
                    num += getWhile( isNumeric );

                    tokens.push_back( { currentRow, currentColumn, TokenType::DOUBLE, std::stod( num ) * ( negative ? -1.0 : 1.0 ) } );
                  } else {
                    // Integer literal
                    tokens.push_back( { currentRow, currentColumn, TokenType::INTEGER, std::stoi( num ) * ( negative ? -1 : 1 ) } );
                  }
                } else if( isAlpha( current ) ) {
                  std::string identifier;
                  identifier += current;
                  identifier += getWhile( [ & ]( char c ) {
                    return isAlpha( c ) || isNumeric( c );
                  } );

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

            currentColumn++;
            negative = false;
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

          throw ParseException();
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
              call.arguments.push_back( AST::Literal{ std::get< int >( tokens.front().metadata ) } );
              increment();
            } else if( checkToken( TokenType::DOUBLE ) ) {
              call.arguments.push_back( AST::Literal{ std::get< double >( tokens.front().metadata ) } );
              increment();
            } else if( checkToken( TokenType::BOOLEAN ) ) {
              call.arguments.push_back( AST::Literal{ std::get< bool >( tokens.front().metadata ) } );
              increment();
            } else if( checkToken( TokenType::STRING ) ) {
              call.arguments.push_back( AST::Literal{ std::get< std::string >( tokens.front().metadata ) } );
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
              identifier.scope.push_back( std::get< std::string >( identifierToken.metadata ) );

              // Pop the SCOPE_RESOLUTION token
              increment();
            } else {
              break;
            }
          }

          Token identToken = getAndExpect( TokenType::IDENTIFIER, "identifier" );
          identifier.value = std::get< std::string >( identToken.metadata );

          return identifier;
        }

        AST::SelectorQuery Parser::getSelectorQuery() {
          AST::SelectorQuery selectorQuery;
          // Hack to make up for the fact that adjusting the parser for whitespace without tests
          // is going to be impossible
          int expectedColumn = -1;

          if( checkToken( TokenType::STAR ) ) {
            selectorQuery.all = true;
            return selectorQuery;
          }

          if( checkToken( TokenType::IDENTIFIER ) ) {
            selectorQuery.tag = std::get< std::string >( tokens.front().metadata );
            expectedColumn = tokens.front().column + 1;
            increment();
          }

          if( checkToken( TokenType::POUND ) ) {
            if( !( expectedColumn != -1 && tokens.front().column != expectedColumn ) ) {
              increment();
              Token identifier = getAndExpect( TokenType::IDENTIFIER, "identifier" );
              selectorQuery.id = std::get< std::string >( identifier.metadata );
              expectedColumn = identifier.column + 1;
            }
          }

          while( checkToken( TokenType::DOT ) ) {
            if( !( expectedColumn != -1 && tokens.front().column != expectedColumn ) ) {
              increment();
              Token identifier = getAndExpect( TokenType::IDENTIFIER, "identifier" );
              selectorQuery.classes.push_back( std::get< std::string >( identifier.metadata ) );
              expectedColumn = identifier.column + 1;
            } else {
              break;
            }
          }

          return selectorQuery;
        }

        AST::Property Parser::getProperty() {
          AST::Property property;

          property.name = std::get< std::string >( tokens.front().metadata );
          increment();

          getAndExpect( TokenType::COLON, ":" );

          if( checkToken( TokenType::INTEGER ) ) {
            property.value = AST::Literal{ std::get< int >( tokens.front().metadata ) };
            increment();
          } else if( checkToken( TokenType::DOUBLE ) ) {
            property.value = AST::Literal{ std::get< double >( tokens.front().metadata ) };
            increment();
          } else if( checkToken( TokenType::BOOLEAN ) ) {
            property.value = AST::Literal{ std::get< bool >( tokens.front().metadata ) };
            increment();
          } else if( checkToken( TokenType::STRING ) ) {
            property.value = AST::Literal{ std::get< std::string >( tokens.front().metadata ) };
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
