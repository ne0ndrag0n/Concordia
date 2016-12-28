#ifndef WALLCELL
#define WALLCELL

#include <memory>

namespace BlueBear {
  namespace Scripting {
    class Wallpaper;

    struct WallCell {

      public:
        struct Segment {
          std::shared_ptr< Wallpaper > front;
          std::shared_ptr< Wallpaper > back;

          public:
            Segment( std::shared_ptr< Wallpaper > front, std::shared_ptr< Wallpaper > back ) : front( front ), back( back ) {}
        };

        std::unique_ptr< Segment > x;
        std::unique_ptr< Segment > y;
        std::unique_ptr< Segment > d;
        std::unique_ptr< Segment > r;
    };

  }
}


#endif
