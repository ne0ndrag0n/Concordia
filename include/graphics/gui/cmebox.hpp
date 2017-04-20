#ifndef CMEBOX
#define CMEBOX

#include "graphics/gui/cmewidget.hpp"
#include <SFGUI/Box.hpp>
#include <memory>
#include <string>

namespace BlueBear {
  namespace Graphics {
    namespace GUI {

      class CMEBox : public sfg::Box, public CMEWidget {
      public:
        CMEBox( sfg::Box::Orientation orientation = sfg::Box::Orientation::HORIZONTAL, float spacing = 0.f );

        static std::shared_ptr< CMEBox > create( sfg::Box::Orientation orientation = sfg::Box::Orientation::HORIZONTAL, float spacing = 0.f );

      };

    }
  }
}

#endif
