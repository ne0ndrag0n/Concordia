#include "graphics/userinterface/style/parser.hpp"
#include "log.hpp"

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
          return ( c >= 'A' && c <= 'Z' ) || ( c >= 'a' && c <= 'z' );
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
          tokens.clear();
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

        AST::PropertyList Parser::getPropertyList() {
          AST::PropertyList rootPropertyList;

          return rootPropertyList;
        }

      }
    }
  }
}
