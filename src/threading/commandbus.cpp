#include "threading/commandbus.hpp"
#include <memory>
#include <mutex>


namespace BlueBear {
  namespace Threading {

    CommandBus::CommandBus() {
      displayCommands = std::make_unique< Graphics::Display::CommandList >();
    }

    void CommandBus::produce( std::unique_ptr< Graphics::Display::CommandList >& source ) {
      std::unique_lock< std::mutex > locker( displayMutex, std::try_to_lock );
      if( locker.owns_lock() ) {
        if( displayCommands->size() == 0 ) {
          source.swap( displayCommands );
        }
      }
    }
    void CommandBus::consume( std::unique_ptr< Graphics::Display::CommandList >& destination ) {
      // Keep this critical section short!
      std::unique_lock< std::mutex > locker( displayMutex, std::try_to_lock );
      if( locker.owns_lock() ) {
        // Okay to do the swap!
        destination.swap( displayCommands );
      }
    }
  }
}
