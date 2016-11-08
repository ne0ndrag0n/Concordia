#ifndef WALLCELLBUNDLER
#define WALLCELLBUNDLER

#include <memory>

namespace BlueBear {
  namespace Graphics {
    class WallInstance;

    class WallCellBundler {
      unsigned int currentRotation;
    public:
      WallCellBundler( unsigned int currentRotation );
      std::shared_ptr< WallInstance > x;
      std::shared_ptr< WallInstance > y;
      std::shared_ptr< WallInstance > d;
      std::shared_ptr< WallInstance > r;

      void render();
    };

  }
}

#endif
