#ifndef STYLE_PARSER
#define STYLE_PARSER

#include "graphics/userinterface/style/ast/propertylist.hpp"
#include "graphics/userinterface/style/ast/property.hpp"
#include "graphics/userinterface/style/ast/selectorquery.hpp"
#include "graphics/userinterface/style/ast/call.hpp"
#include "graphics/userinterface/style/ast/identifier.hpp"
#include "graphics/userinterface/style/token.hpp"
#include "exceptions/genexc.hpp"
#include <functional>
#include <string>
#include <istream>
#include <list>
#include <vector>
#include <memory>

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {
      namespace Style {

        class Parser {
          std::string path;
          std::unique_ptr< std::istream > file;
          std::list< Token > tokens;
          unsigned int currentRow;
          unsigned int currentColumn;

          bool checkAndAdvanceChar( char expect );
          static bool isAlpha( char c );
          static bool isNumeric( char c );
          std::string getWhile( std::function< bool( char ) > predicate );
          void getTokens();

          bool checkToken( TokenType expectedType );
          bool checkToken( std::list< Token >::iterator iterator, TokenType expectedType );
          bool lookaheadAndCheckToken( std::list< Token >::iterator iterator, unsigned int lookahead, TokenType expectedType );
          Token getAndExpect( TokenType expectedType, const std::string& expectation );
          void increment();
          void throwParseException( const std::string& expectation );
          bool isSelectorToken();
          AST::Identifier getIdentifier();
          AST::Call getCall();
          AST::SelectorQuery getSelectorQuery();
          AST::Property getProperty();
          AST::PropertyList getPropertyList();

        public:
          EXCEPTION_TYPE( LexException, "Invalid character while parsing" );
          EXCEPTION_TYPE( ParseException, "Invalid token while parsing" );

          Parser( const std::string& path, bool isSnippet = false );

          std::vector< AST::PropertyList > getStylesheet();
        };

      }
    }
  }
}

#endif
