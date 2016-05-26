#include "eventmanager.hpp"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "utility.hpp"
#include "lot.hpp"
#include <map>
#include <string>
#include <iostream>

namespace BlueBear {

  EventManager::EventManager( lua_State* L, std::shared_ptr< Lot > currentLot ) : L( L ), currentLot( currentLot ) {}

  void EventManager::reset() {
    events.clear();
  }

  void EventManager::registerEvent( const std::string& eventKey, const std::string& cid, const std::string& callback ) {
    int object = currentLot->getLotObjectByCid( cid );

    // Can't do anything without a valid object ID
    if( object != -1 ) {
      if( !events.count( eventKey ) ) {
        // Nested map not created
        events[ eventKey ] = EventMap();
      }

      events[ eventKey ][ object ] = callback;
    }
  }

  void EventManager::unregisterEvent( const std::string& eventKey, const std::string& cid ) {
    int object = currentLot->getLotObjectByCid( cid );

    // Can't do anything without a valid object ID
    if( object != -1 ) {
      if( events.count( eventKey ) ) {
        auto eventMap = events[ eventKey ];

        eventMap.erase( object );

        // If we removed the last event in eventsMap
        // erase the eventsMap
        if( eventMap.size() == 0 ) {
          events.erase( eventKey );
        }
      }
    }
  }

  void EventManager::broadcastEvent( const std::string& eventKey ) {
    if( events.count( eventKey ) ) {
      auto listeners = events[ eventKey ];
    }
  }

  /**
   * listen_for( "fully-qualified-event-key", self._cid, "func_name" )
   */
  int EventManager::lua_registerEvent( lua_State* L ) {
    EventManager& eventManager = *( ( EventManager* )lua_touserdata( L, lua_upvalueindex( 1 ) ) );

    if( lua_isstring( L, -1 ) && lua_isstring( L, -2 ) && lua_isstring( L, -3 ) ) {

      std::string eventKey( lua_tostring( L, -3 ) );
      std::string cid( lua_tostring( L, -2 ) );
      std::string callback( lua_tostring( L, -1 ) );

      eventManager.registerEvent( eventKey, cid, callback );
    }

    return 0;
  }

  /**
	 * stop_listening_for( "fully-qualified-event-key", self._cid )
	 */
	int EventManager::lua_unregisterEvent( lua_State* L ) {
    EventManager& eventManager = *( ( EventManager* )lua_touserdata( L, lua_upvalueindex( 1 ) ) );

		if( lua_isstring( L, -2 ) && lua_isstring( L, -1 ) ) {

			std::string eventKey( lua_tostring( L, -2 ) );
			std::string cid( lua_tostring( L, -1 ) );

			eventManager.unregisterEvent( eventKey, cid );
		}

		return 0;
	}

  /**
	 * broadcast( "fully-qualified-event-key", ... )
	 */
	int EventManager::lua_broadcastEvent( lua_State* L ) {
    EventManager& eventManager = *( ( EventManager* )lua_touserdata( L, lua_upvalueindex( 1 ) ) );

		return 0;
	}

  /**
   * Stemcell stores this on the userdata metatable in __gc to ensure the instance gets deleted
   */
   int EventManager::lua_gc( lua_State* L ) {
     EventManager** eventManager = ( EventManager** )lua_touserdata( L, 1 );
     delete *eventManager;
     std::cout << "Garbage collected an EventManager object" << std::endl;
     return 0;
   }

}
