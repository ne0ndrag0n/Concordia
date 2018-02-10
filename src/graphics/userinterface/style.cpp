#include "graphics/userinterface/style.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {

      Style::Value Style::getValue( const std::string& id ) {
        return values.at( id );
      }

    }
  }
}
