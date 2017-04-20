#include "graphics/gui/cmealignment.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace GUI {

      std::shared_ptr< CMEAlignment > CMEAlignment::create() {
        return std::make_shared< CMEAlignment >();
      }

    }
  }
}
