#ifndef EVENT_MANAGER
#define EVENT_MANAGER

#include "bbtypes.hpp"
#include <map>
#include <functional>

namespace BlueBear {

  /**
   * This really is not what I wanted and I hope it doesn't bite me in the ass down the line.
   */
  class UIActionEvent {
    std::map< void*, std::function< void( LuaReference ) > > listeners;

  public:
    /**
     * Listen for a specific signal
     */
    void listen( void* key, std::function< void( LuaReference ) > callback );

    /**
     * Stop listening for a specific signal
     */
    void stopListening( void* key );

    /**
     * Trigger all listeners for a specific signal
     */
    void trigger( LuaReference param );
  };

  /**
   * This is mainly meant to be a many-to-one mapping for internal use
   */
  class SFGUISignalEvent {
  public:
    enum class Event {
      DISABLE_KEYBOARD_EVENTS,
      ENABLE_KEYBOARD_EVENTS
    };

    void listen( Event event, std::function< void() > callback );

    void stopListening( Event event );

    void trigger( Event event );

  private:
    std::map< Event, std::function< void() > > listeners;
  };

  struct EventManager {
    UIActionEvent UI_ACTION_EVENT;
    SFGUISignalEvent SFGUI_SIGNAL_EVENT;
  };

  // eventManager.UI_ACTION_EVENT.listen( this, [ & ]( LuaReference luaref ) { engine queues up passed-in lua reference } )
  // eventManager.UI_ACTION_EVENT.trigger( 3 )
}



#endif
