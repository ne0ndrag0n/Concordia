#include "graphics/gui/cmelabel.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace GUI {

      CMELabel::CMELabel( const std::string& text ) : sfg::Label( text ) {}

      std::shared_ptr< CMELabel > CMELabel::create( const std::string& text ) {
        return std::make_shared< CMELabel >( text );
      }

    }
  }
}
