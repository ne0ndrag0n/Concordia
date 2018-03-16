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
          if( tokens.empty() ) {
            return false;
          }

          return tokens.front().type == expectedType;
        }

        Token Parser::getAndExpect( TokenType expectedType, const std::string& expectation ) {
          if( !checkToken( expectedType ) ) {
            throwParseException( expectation );
          }

          Token next = tokens.front();
          tokens.pop_front();
          return next;
        }

        bool Parser::isSelectorToken() {
          return checkToken( TokenType::IDENTIFIER ) || checkToken( TokenType::POUND ) ||
            checkToken( TokenType::DOT ) || checkToken( TokenType::STAR );
        }

        AST::SelectorQuery Parser::getSelectorQuery() {
          AST::SelectorQuery selectorQuery;

          if( checkToken( TokenType::STAR ) ) {
            selectorQuery.all = true;
            return selectorQuery;
          }

          if( checkToken( TokenType::IDENTIFIER ) ) {
            selectorQuery.tag = std::any_cast< std::string >( tokens.front().metadata );
            tokens.pop_front();
          }

          if( checkToken( TokenType::POUND ) ) {
            tokens.pop_front();
            selectorQuery.id = std::any_cast< std::string >( getAndExpect( TokenType::IDENTIFIER, "identifier" ) );
          }

          while( checkToken( TokenType::DOT ) ) {
            tokens.pop_front();
            selectorQuery.classes.push_back(
              std::any_cast< std::string >( getAndExpect( TokenType::IDENTIFIER, "identifier" ) )
            );
          }

          return selectorQuery;
        }

        AST::Property Parser::getProperty() {
          AST::Property property;

          property.name = std::any_cast< std::string >( tokens.front().metadata );
          tokens.pop_front();

          if( checkToken( TokenType::INTEGER ) ) {
            property.value = AST::Literal{ std::any_cast< int >( tokens.front().metadata ) };
            tokens.pop_front();
          } else if( checkToken( TokenType::DOUBLE ) ) {
            property.value = AST::Literal{ std::any_cast< double >( tokens.front().metadata ) };
            tokens.pop_front();
          } else if( checkToken( TokenType::BOOLEAN ) ) {
            property.value = AST::Literal{ std::any_cast< bool >( tokens.front().metadata ) };
            tokens.pop_front();
          } else if( checkToken( TokenType::STRING ) ) {
            property.value = AST::Literal{ std::any_cast< std::string >( tokens.front().metadata ) };
            tokens.pop_front();
          } else if( checkToken( TokenType::IDENTIFIER ) ) {
            // Call - IDENTIFIER ( SCOPE_RESOLUTION IDENTIFIER )* LEFT_PAREN
            // Else plain old identifier
            auto iterator = tokens.begin();
            std::advance( iterator, 1 );

            if( iterator->type == TokenType::LEFT_PAREN ) {
              // Skip directly to function call
              // property.value = getCall();
            } else if( iterator->type == TokenType::SCOPE_RESOLUTION ) {
              // Get scope resolution and then check for left_paren after
              // If a left_paren is not present after all that, then we use the identifier directly
              auto scope = tokens.begin();
              do {

              } while( true );
            } else {

            }
          } else {
            throwParseException( "integer, double, boolean, string, or identifier token" );
          }

          return property;
        }

        AST::PropertyList Parser::getPropertyList() {
          AST::PropertyList rootPropertyList;

          // Get the selector
          if( isSelectorToken() ) {
            do {
              rootPropertyList.selectorQueries.push_back( getSelectorQuery() );
            } while( isSelectorToken() );
          } else {
            throwParseException( "identifier, #, ., or *" );
          }

          getAndExpect( TokenType::LEFT_BRACE, "{" );

          // identifier plus colon is a property token
          if( checkToken( TokenType::IDENTIFIER ) ) {
            auto afterFront = tokens.begin();
            std::advance( afterFront, 1 );

            if( afterFront != tokens.end() && afterFront->type == TokenType::DOT ) {
              // Still on identifier here!
              rootPropertyList.properties.push_back( getProperty() );
            }
          } else if( isSelectorToken() ) {
            rootPropertyList.children.push_back( getPropertyList() );
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
