#include "scripting/eventmanager.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "tools/utility.hpp"
#include "scripting/lot.hpp"
#include "scripting/lotentity.hpp"
#include "log.hpp"
#include <jsoncpp/json/json.h>
#include <cstddef>
#include <vector>
#include <map>
#include <string>
#include <iostream>

namespace BlueBear {
  namespace Scripting {

  EventManager::EventManager( lua_State* L, std::shared_ptr< Lot > currentLot ) : L( L ), currentLot( currentLot ) {}

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

  /**
   * This function assumes you have pushed either nil or a table onto the Lua stack. This represents the "arguments"
   * passed along to each event.
   *
   * If there is no callback to be called, the value will simply be popped whenever LotEntity::registerCallback
   * is called. Therefore, these two values must be copied on the stack every time a registerCallback call is made.
   */
  void EventManager::broadcastEvent( const std::string& eventKey ) {
    if( events.count( eventKey ) ) {
      auto listeners = events[ eventKey ];
      std::vector< std::string > deletions;

      for( const auto& keyValuePairs : listeners ) {
        const std::string& cid = keyValuePairs.first;
        const std::string& callback = keyValuePairs.second;

        // On a global event manager, this should always succeed, but it may not succeed on an individual event manager.
        // If it doesn't succeed, the object no longer exists, and we should simply let this drop off.
        if( currentLot->objects.count( cid ) ) {
          LotEntity& entity = *( currentLot->objects[ cid ] );

          // Copy the item at the top of stack. It should be either table or nil, but there should have been
          // something you put here before calling broadcastEvent. If you don't push something, the behaviour
          // is undefined.
          lua_pushvalue( L, -1 );

          // This call will consume the copied item.
          entity.deferCallback( callback );
        } else {
          // This has to be deleted later.
          deletions.push_back( cid );
        }
      }

      // After we're done with that, delete what is due to be deleted (we couldn't do it *in* the loop)
      for( std::string& deletion : deletions ) {
        listeners.erase( deletion );
      }
    }

    // Remember that nil or table that you pushed?
    lua_pop( L, 1 );
  }

  /**
   * Create the EventManager state from a serialised EventManager. Easy enough, we store them in a map format similar to how
   * you'd otherwise represent it in JSON.
   */
  void EventManager::load( Json::Value& serializedEventManager ) {
    // top level: event: callbackset, event: callbackset, ...
    for( Json::Value::iterator jsonIterator = serializedEventManager.begin(); jsonIterator != serializedEventManager.end(); ++jsonIterator ) {
      std::string eventKey = jsonIterator.key().asString();
      Json::Value eventMap = *jsonIterator;

      // second level: instance: callback, instance: callback, ...
      for( Json::Value::iterator innerIterator = eventMap.begin(); innerIterator != eventMap.end(); ++innerIterator ) {
        std::string cid = innerIterator.key().asString();
        std::string callback = ( *innerIterator ).asString();

        registerEvent( eventKey, cid, callback );
      }
    }
  }

  /**
   * load( table )
   */
   int EventManager::lua_load( lua_State* L ) {
     EventManager& eventManager = *( ( EventManager* )lua_touserdata( L, lua_upvalueindex( 1 ) ) );

     // Here, we'll need to leverage the JSON lib to convert the lua table provided to JSON, then
     // convert that string representation to a Json::Value and hand that into eventManager.load

     // table
     if( lua_istable( L, -1 ) ) {
       // JSON table
       lua_getglobal( L, "JSON" );

       // encode() JSON table
       Tools::Utility::getTableValue( L, "encode" );

       // JSON encode() JSON table
       lua_pushvalue( L, -2 );

       // table JSON encode() JSON table
       lua_pushvalue( L, -4 );

       if( !lua_pcall( L, 2, 1, 0 ) ) {
         // "table as json string" JSON table
         // just a test for now...
         Json::Value json;
         static Json::Reader reader;
         if( reader.parse( lua_tostring( L, -1 ), json ) ) {
           eventManager.load( json );
         } else {
           return luaL_error( L, "EventManager: Failed to parse JSON for serialised EventManager." );
         }
       } else {
         // error JSON table
         return luaL_error( L, "EventManager: Failed to convert table to JSON: %s", lua_tostring( L, -1 ) );
       }
     } else {
       return luaL_error( L, "EventManager: Did not provide a valid serialised instance to load()!" );
     }

     return 0;
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

    // The item way at the bottom of the stack has the event and should be a string
    int arguments = lua_gettop( L );
    if( !arguments || !lua_isstring( L, -arguments ) ) {
      return luaL_error( L, "Did not provide an event key to broadcast()!" );
    }

    std::string eventKey = lua_tostring( L, -arguments );

    // Decide whether or not we need to push a nil, or push a table packed with arguments
    if( arguments == 1 ) {
      // If there are no arguments, don't waste time
      lua_pushnil( L );
    } else {
      // If there are arguments, package them up
      lua_newtable( L );
      arguments++;
      for( int stackIndex = -arguments + 1; stackIndex != -1; stackIndex++ ) {
        // Bring the value to the top
        lua_pushvalue( L, stackIndex );

        // Push the value onto the table below
        lua_rawseti( L, -2, arguments + stackIndex );
      }
    }

    // Broadcasts an async event where listeners will be executed on the next tick
    eventManager.broadcastEvent( eventKey );

		return 0;
	}

  /**
   * Stemcell stores this on the userdata metatable in __gc to ensure the instance gets deleted
   */
   int EventManager::lua_gc( lua_State* L ) {
     EventManager** eventManager = ( EventManager** )lua_touserdata( L, 1 );
     delete *eventManager;
     Log::getInstance().debug( "EventManager::lua_gc", "Garbage collected an EventManager object" );
     return 0;
   }
 }
}
