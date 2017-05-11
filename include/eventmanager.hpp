#ifndef EVENT_MANAGER
#define EVENT_MANAGER

#include "bbtypes.hpp"
#include <map>
#include <functional>

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
   * This is mainly meant to be a many-to-one mapping for internal use
   */
  class SFGUIEatEvent {
  public:
    enum class Event {
      EAT_KEYBOARD_EVENT,
      EAT_MOUSE_EVENT
    };

    void listen( Event event, std::function< void() > callback );

    void stopListening( Event event );

    void trigger( Event event );

  private:
    std::map< Event, std::function< void() > > listeners;
  };

  struct EventManager {
    BasicEvent< void*, LuaReference > UI_ACTION_EVENT;
    SFGUIEatEvent SFGUI_EAT_EVENT;
    BasicEvent< void*, unsigned int > ITEM_ADDED;
    BasicEvent< void*, unsigned int > ITEM_REMOVED;
  };

  // eventManager.UI_ACTION_EVENT.listen( this, [ & ]( LuaReference luaref ) { engine queues up passed-in lua reference } )
  // eventManager.UI_ACTION_EVENT.trigger( 3 )
}



#endif
