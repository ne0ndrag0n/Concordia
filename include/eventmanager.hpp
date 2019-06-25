#ifndef EVENT_MANAGER
#define EVENT_MANAGER

#include "bbtypes.hpp"
#include <map>
#include <functional>
#include <memory>
#include <sol.hpp>

namespace BlueBear {

  /**
   * This is a standard, flexible base event type that can be used for anything that takes a listen on an instance,
   * and fires off a callback with varying types.
   */
  template < typename Key, typename... Signature > class BasicEvent {
    std::map< Key, std::function< void( Signature... ) > > listeners;

    public:
      void listen( Key key, std::function< void( Signature... ) > callback ) {
        listeners[ key ] = callback;
      }
      void stopListening( Key key ) {
        listeners.erase( key );
      }
      void trigger( Signature... params ) {
        for( auto& nestedPair : listeners ) {
          nestedPair.second( params... );
        }
      }
  };

  /**
   * This is an event type similar to BasicEvent, but allows you to perform an action for every object
   * which is triggered. This essentially inverts BasicEvent to perform an action on the trigger side
   * for every object that is triggered.
   */
   template< typename Key, typename... Signature > class SelectableActionEvent {
     std::map< Key, std::function< bool( Signature... ) > > predicates;

   public:
     void listen( Key key, std::function< bool( Signature... ) > predicate ) {
       predicates[ key ] = predicate;
     }

     void stopListening( Key key ) {
       predicates.erase( key );
     }

     void trigger( std::function< void( Key ) > action, Signature... params ) {
       for( auto& nestedPair : predicates ) {
         if( nestedPair.second( params... ) ) {
           return action( nestedPair.first );
         }
       }
     }

     void triggerAll( std::function< void( Key ) > action, Signature... params ) {
       for( auto& nestedPair : predicates ) {
         if( nestedPair.second( params... ) ) {
           action( nestedPair.first );
         }
       }
     }
   };

  struct EventManager {
    BasicEvent< void*, std::string > MESSAGE_LOGGED;
    BasicEvent< void*, sol::state& > LUA_STATE_READY;
  };

  extern EventManager eventManager;

  // eventManager.UI_ACTION_EVENT_LEGACY.listen( this, [ & ]( LuaReference luaref ) { engine queues up passed-in lua reference } )
  // eventManager.UI_ACTION_EVENT_LEGACY.trigger( 3 )
}



#endif
