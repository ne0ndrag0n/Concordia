#ifndef CMEWINDOW
#define CMEWINDOW

#include "graphics/gui/cmewidget.hpp"
#include <SFGUI/Window.hpp>
#include <memory>
#include <string>

namespace BlueBear {
  namespace Graphics {
    namespace GUI {

      class CMEWindow : public sfg::Window, public CMEWidget {
      public:
        CMEWindow( char style );

        static std::shared_ptr< CMEWindow > create( char style = sfg::Window::Style::TOPLEVEL );

      };

    }
  }
}

#endif
