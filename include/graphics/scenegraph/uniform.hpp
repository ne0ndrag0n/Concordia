#ifndef SG_BASIC_UNIFORM
#define SG_BASIC_UNIFORM

namespace BlueBear {
  namespace Graphics {
    class Shader;

    namespace SceneGraph {

        class Uniform {
        public:
          virtual void update();
          virtual void send( const Shader& shader ) = 0;
        };

    }
  }
}

#endif
