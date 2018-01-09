#ifndef ENVIRONMENT_INTERFACE
#define ENVIRONMENT_INTERFACE

#include <memory>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      class Model;
    }
  }

  namespace Models {

    class Environment {
    public:
      virtual std::shared_ptr< Graphics::SceneGraph::Model > generateEnvironment() = 0;
    };

  }
}

#endif
