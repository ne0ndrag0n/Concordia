#include "eventmanager.hpp"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include <map>
#include <string>

namespace BlueBear {

  EventManager::EventManager( lua_State* L ) : L( L ) {}

  void EventManager::reset() {
    events.clear();
  }

  void EventManager::registerEvent( const std::string& eventKey, const std::string& cid, const std::string& callback ) {
    if( !events.count( eventKey ) ) {
			// Nested map not created
			events[ eventKey ] = EventMap();
		}

		events[ eventKey ][ cid ] = callback;
  }

  void EventManager::unregisterEvent( const std::string& eventKey, const std::string& cid ) {
    if( events.count( eventKey ) ) {
      auto eventMap = events[ eventKey ];

      eventMap.erase( cid );

      // If we removed the last event in eventsMap
      // erase the eventsMap
      if( eventMap.size() == 0 ) {
        events.erase( eventKey );
      }
    }
  }

  void EventManager::broadcastEvent( const std::string& eventKey ) {
    if( events.count( eventKey ) ) {
      auto listeners = events[ eventKey ];
    }
  }
}
