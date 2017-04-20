#include "graphics/gui/cmewindow.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace GUI {

      CMEWindow::CMEWindow( char style ) : sfg::Window( style ) {}

      std::shared_ptr< CMEWindow > CMEWindow::create( char style ) {
        return std::make_shared< CMEWindow >( style );
      }

    }
  }
}
