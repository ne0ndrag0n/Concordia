#ifndef SG_ANIMATION_ANIMATION
#define SG_ANIMATION_ANIMATION

#include <string>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      namespace Animation {

        struct Animation {
          std::string id;
          double fps;
          double duration;
        };

      }
    }
  }
}

#endif
