#include "graphics/wallcellbundler.hpp"
#include "graphics/instance/wallinstance.hpp"

namespace BlueBear {
  namespace Graphics {

    WallCellBundler::WallCellBundler( unsigned int currentRotation ) : currentRotation( currentRotation ) {}

    void WallCellBundler::render() {
      if( x ) {
        x->drawEntity();
      }

      if( y ) {
        y->drawEntity();
      }
    }

  }
}
