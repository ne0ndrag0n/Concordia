#ifndef SG_BASIC_UNIFORM
#define SG_BASIC_UNIFORM

#include <memory>

namespace BlueBear {
  namespace Graphics {
    class Shader;

    namespace SceneGraph {

        class Uniform {
        public:
          virtual void update();
          virtual void send( const Shader& shader ) = 0;
          virtual std::unique_ptr< Uniform > copy() = 0;
        };

    }
  }
}

#endif
