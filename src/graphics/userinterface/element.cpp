#include "graphics/userinterface/element.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {

      Element::Element( const std::string& tag, const std::string& id, const std::vector< std::string >& classes ) : tag( tag ), id( id ), classes( classes ) {}

      std::shared_ptr< Element > Element::create( const std::string& tag, const std::string& id, const std::vector< std::string >& classes ) {
        std::shared_ptr< Element > result( new Element( tag, id, classes ) );

        result->style.parent = result;

        return result;
      }

    }
  }
}
