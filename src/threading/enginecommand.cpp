#include "threading/enginecommand.hpp"
#include "graphics/entity.hpp"
#include "scripting/engine.hpp"
#include <memory>

namespace BlueBear {
  namespace Threading {
    namespace Engine {

      RegisterInstance::RegisterInstance( unsigned int instanceId )
        : instanceId( instanceId ) {}

      void RegisterInstance::execute( Scripting::Engine& instance ) {

      }
    }
  }
}
