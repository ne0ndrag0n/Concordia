#ifndef SG_BASIC_UNIFORM
#define SG_BASIC_UNIFORM

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {

        class Uniform {
        public:
          virtual void update();
          virtual void send() = 0;
        };

    }
  }
}

#endif
