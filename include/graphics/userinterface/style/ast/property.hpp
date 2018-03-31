#ifndef PARSER_PROPERTY
#define PARSER_PROPERTY

#include "graphics/userinterface/style/ast/call.hpp"
#include "graphics/userinterface/style/ast/literal.hpp"
#include "graphics/userinterface/style/ast/identifier.hpp"
#include <string>
#include <variant>

#include "log.hpp"
#include "tools/utility.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {
      namespace Style {
        namespace AST {

          struct Property {
            std::string name;
            std::variant< Call, Identifier, Literal > value;

            void print( unsigned int indentation = 0 ) const {
              std::string selected;
              if( std::holds_alternative< Call >( value ) ) {
                selected = "<call>";
              } else if( std::holds_alternative< Literal >( value ) ) {
                selected = "<literal>";
              } else if( std::holds_alternative< Identifier >( value ) ) {
                selected = "<identifier>";
              } else {
                Log::getInstance().error( "Property::print", Tools::Utility::generateIndentation( indentation ) + "This should never happen" );
              }

              Log::getInstance().debug( "Property::print", Tools::Utility::generateIndentation( indentation ) + name + ": " + selected );
            };
          };

        }
      }
    }
  }
}

#endif
