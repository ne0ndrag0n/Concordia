#ifndef WALLCELL
#define WALLCELL

namespace BlueBear {
  namespace Scripting {

    struct WallCell {

      public:
        struct Segment {
          std::shared_ptr< Wallpaper > front;
          std::shared_ptr< Wallpaper > back;
        };

        std::unique_ptr< Segment > x;
        std::unique_ptr< Segment > y;
        std::unique_ptr< Segment > d;
        std::unique_ptr< Segment > r;
    };

  }
}


#endif
