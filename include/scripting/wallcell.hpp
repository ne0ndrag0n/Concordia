#ifndef WALLCELL
#define WALLCELL

#include "threading/lockable.hpp"
#include <memory>

namespace BlueBear {
  namespace Scripting {
    class Wallpaper;

    struct WallCell {

      public:
        struct Segment {
          Threading::Lockable< Wallpaper > front;
          Threading::Lockable< Wallpaper > back;

          public:
            Segment( Threading::Lockable< Wallpaper > front, Threading::Lockable< Wallpaper > back ) : front( front ), back( back ) {}
        };

        std::unique_ptr< Segment > x;
        std::unique_ptr< Segment > y;
        std::unique_ptr< Segment > d;
        std::unique_ptr< Segment > r;
    };

  }
}


#endif
