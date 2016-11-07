#ifndef WALLCELLBUNDLER
#define WALLCELLBUNDLER

namespace BlueBear {
  namespace Graphics {

    class WallCellBundler {
    public:
      std::shared_ptr< WallInstance > x;
      std::shared_ptr< WallInstance > y;
      std::shared_ptr< WallInstance > d;
      std::shared_ptr< WallInstance > r;
    };

  }
}

#endif
