#ifndef EVENTMANAGER
#define EVENTMANAGER

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "lot.hpp"
#include <map>
#include <string>
#include <memory>

namespace BlueBear {
  class EventManager {

    using EventMap = std::map< std::string, std::string >;

    private:
      lua_State* L;
      std::map< std::string, EventMap > events;
      std::shared_ptr< Lot > currentLot;

    public:
      EventManager( lua_State* L, std::shared_ptr< Lot > currentLot );
      void reset();
      void registerEvent( const std::string& eventKey, const std::string& cid, const std::string& callback );
      void unregisterEvent( const std::string& eventKey, const std::string& cid );
      void broadcastEvent( const std::string& eventKey );

      static int lua_registerEvent( lua_State* L );
      static int lua_unregisterEvent( lua_State* L );
      static int lua_broadcastEvent( lua_State* L );
      static int lua_gc( lua_State* L );
  };

}

#endif
