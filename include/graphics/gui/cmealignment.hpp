#ifndef CMEALIGNMENT
#define CMEALIGNMENT

#include "graphics/gui/cmewidget.hpp"
#include <SFGUI/Alignment.hpp>
#include <memory>
#include <string>

namespace BlueBear {
  namespace Graphics {
    namespace GUI {

      class CMEAlignment : public sfg::Alignment, public CMEWidget {
      public:
        static std::shared_ptr< CMEAlignment > create();
      };

    }
  }
}

#endif
