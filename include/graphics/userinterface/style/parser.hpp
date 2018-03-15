#ifndef STYLE_PARSER
#define STYLE_PARSER

#include "graphics/userinterface/style/ast/propertylist.hpp"
#include "graphics/userinterface/style/token.hpp"
#include "exceptions/genexc.hpp"
#include <functional>
#include <string>
#include <fstream>
#include <list>

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {
      namespace Style {

        class Parser {
          std::string path;
          std::ifstream file;
          std::list< Token > tokens;
          unsigned int currentRow = 0;
          unsigned int currentColumn = 0;

          bool checkAndAdvanceChar( char expect );
          static bool isAlpha( char c );
          static bool isNumeric( char c );
          std::string getWhile( std::function< bool( char ) > predicate );
          void getTokens();

        public:
          EXCEPTION_TYPE( LexException, "Invalid character while parsing" );
          EXCEPTION_TYPE( ParseException, "Invalid token while parsing" );

          Parser( const std::string& path );

          AST::PropertyList getPropertyList();
        };

      }
    }
  }
}

#endif
