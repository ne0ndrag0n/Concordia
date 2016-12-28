#ifndef WAITINGTABLE
#define WAITINGTABLE

#include "bbtypes.hpp"
#include <jsoncpp/json/json.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <map>
#include <list>
#include <queue>
#include <string>
#include <unordered_set>

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

        std::queue< LuaReference >& engineQueue;

        std::map< Tick, std::list< LuaReference > > timerMap;

        std::list< LuaReference >& getBucket( Tick key );

      public:
        WaitingTable( std::queue< LuaReference >& engineQueue );

        std::unordered_set< LuaReference > loadFromJSON( Json::Value& loadingTable, std::map< std::string, LuaReference >& entities );
        Json::Value saveToJSON( lua_State* L );

        std::string waitForTick( Tick deadline, LuaReference function );
        void cancelTick( const std::string& handle );
        void triggerTick( Tick tick );

      };

    }
  }
}
#endif
