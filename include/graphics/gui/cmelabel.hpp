#ifndef CMELABEL
#define CMELABEL

#include "graphics/gui/cmewidget.hpp"
#include <SFGUI/Label.hpp>
#include <memory>
#include <string>

namespace BlueBear {
  namespace Graphics {
    namespace GUI {

      class CMELabel : public sfg::Label, public CMEWidget {
      public:
        CMELabel( const std::string& text );

        static std::shared_ptr< CMELabel > create( const std::string& text = "" );

      };

    }
  }
}

#endif
