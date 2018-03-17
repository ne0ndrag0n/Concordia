#ifndef STYLE_TOKEN
#define STYLE_TOKEN

#include <variant>
#include <string>

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {
      namespace Style {

        enum class TokenType {
          POUND, DOT, STAR, IDENTIFIER, LEFT_BRACE, RIGHT_BRACE,

          INTEGER, DOUBLE, STRING, BOOLEAN, SEMICOLON, COMMA,
          COLON, SCOPE_RESOLUTION, LEFT_PAREN, RIGHT_PAREN
        };

        struct Token {
          unsigned int row;
          unsigned int column;
          TokenType type;
          std::variant< int, double, std::string, bool > metadata;
        };

      }
    }
  }
}

#endif
