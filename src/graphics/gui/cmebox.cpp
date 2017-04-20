#include "graphics/gui/cmebox.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace GUI {

      CMEBox::CMEBox( sfg::Box::Orientation orientation, float spacing ) : sfg::Box( orientation, spacing ) {}

      std::shared_ptr< CMEBox > CMEBox::create( sfg::Box::Orientation orientation, float spacing ) {
        return std::make_shared< CMEBox >( orientation, spacing );
      }

    }
  }
}
