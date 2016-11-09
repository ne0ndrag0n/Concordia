#include "graphics/wallcellbundler.hpp"
#include "graphics/instance/wallinstance.hpp"
#include "graphics/model.hpp"
#include <memory>

namespace BlueBear {
  namespace Graphics {

    std::shared_ptr< Model > WallCellBundler::Piece( nullptr );

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
