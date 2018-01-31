#ifndef WAITINGTABLE
#define WAITINGTABLE

#include "bbtypes.hpp"
#include <jsoncpp/json/json.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <sol.hpp>
#include <map>
#include <list>
#include <queue>
#include <string>

namespace BlueBear {
  namespace Scripting {
    namespace Event {

      /**
       * This class holds a reference to the queue and injects items as-needed, as-requested. There are two types of functions:
       * - waitFor* methods: Given all required information, wait for some specified event to occur. Some of these may return
       *   cancelable handles you can use for the cancel() methods.
       * - trigger* methods: Trigger all types of event that match the given information. This will plant items ready-to-go in preperation
       *   for the next tick iteration.
       * - cancel* methods: Unregister a specific kind of element.
       */
      class WaitingTable {

        std::map< Tick, std::list< sol::function > > timerMap;

        std::list< sol::function >& getBucket( Tick key );

      public:
        std::queue< sol::function > queuedCallbacks;

        void loadFromJSON(
          Json::Value& loadingTable,
          std::map< std::string, LuaReference >& entities,
          sol::state& lua
        );
        Json::Value saveToJSON( lua_State* L );

        std::string waitForTick( Tick deadline, sol::function function );
        void cancelTick( const std::string& handle );
        void triggerTick( Tick tick );

      };

    }
  }
}
#endif
