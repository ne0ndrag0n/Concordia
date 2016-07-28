#include "threading/enginecommand.hpp"
#include "graphics/entity.hpp"
#include "scripting/engine.hpp"
#include "log.hpp"
#include <memory>
#include <sstream>
#include <string>

namespace BlueBear {
  namespace Threading {
    namespace Engine {

      RegisterInstance::RegisterInstance( unsigned int instanceId )
        : instanceId( instanceId ) {}

      void RegisterInstance::execute( Scripting::Engine& instance ) {
        std::stringstream ss;
        ss << "Registered this new entity " << instanceId;
        Log::getInstance().info( "RegisterInstance", ss.str() );
      }
    }
  }
}
