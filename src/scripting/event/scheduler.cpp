#include "scripting/event/scheduler.hpp"

namespace BlueBear {
  namespace Scripting {
    namespace Event {

      Scheduler::Scheduler( lua_State* L ) : L( L ) {}

      void Scheduler::processTick( Tick& currentTick ) {
        // Handle the event loop, bring items from upcoming into queue, etc, etc
      }

    }
  }
}
