#ifndef EVENTQUEUE
#define EVENTQUEUE

#include "bbtypes.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <map>
#include <vector>
#include <queue>

namespace BlueBear {
  namespace Scripting {
    namespace LuaKit {
      class Serializer;
    }

    namespace Event {

      class Scheduler {

        lua_State* L;
        std::queue< LuaReference > queue;

        friend class LuaKit::Serializer;

      public:
        Scheduler( lua_State* L );

        /**
         * Complete one tick's worth of callbacks. If there are no callbacks in "messages",
         * the tick will simply be allowed to elapse (up to the tick limit per second).
         */
        void processTick( Tick& currentTick );

      };

    }
  }
}



#endif
