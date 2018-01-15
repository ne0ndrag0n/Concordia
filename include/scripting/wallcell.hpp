#ifndef WALLCELL
#define WALLCELL

#include <memory>

namespace BlueBear {
  namespace Models {
    class Wallpaper;
  }

  namespace Scripting {
    struct WallCell {

      public:
        struct Segment {
          std::shared_ptr< Models::Wallpaper > front;
          std::shared_ptr< Models::Wallpaper > back;

          public:
            Segment( std::shared_ptr< Models::Wallpaper > front, std::shared_ptr< Models::Wallpaper > back ) : front( front ), back( back ) {}
        };

        std::unique_ptr< Segment > x;
        std::unique_ptr< Segment > y;
        std::unique_ptr< Segment > d;
        std::unique_ptr< Segment > r;
    };

  }
}


#endif
