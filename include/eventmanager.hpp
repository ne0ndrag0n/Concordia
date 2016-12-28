#ifndef EVENT_MANAGER
#define EVENT_MANAGER

#include <map>
#include <functional>

namespace BlueBear {

  class EventManager {
  public:
    using Callback = std::function< void() >;
    using Bucket = std::map< void*, Callback >;

    using KeyType = void*;

    enum class Signal {
      UI_EVENT
    };

    /**
     * Listen for a specific signal
     */
    void listen( Signal signal, KeyType key, Callback callback );

    /**
     * Stop listening for a specific signal
     */
    void stopListening( Signal signal, KeyType key );

    /**
     * Trigger all listeners for a specific signal
     */
    void trigger( Signal signal );

  private:
    std::map< Signal, Bucket > signals;
  };

}



#endif
