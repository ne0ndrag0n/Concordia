#include "threading/commandbus.hpp"
#include "threading/displaycommand.hpp"
#include <memory>
#include <mutex>


namespace BlueBear {
  namespace Threading {

    CommandBus::CommandBus() {
      displayCommands = std::make_unique< Display::CommandList >();
    }

    void CommandBus::produce( std::unique_ptr< Display::CommandList >& source ) {
      std::unique_lock< std::mutex > locker( displayMutex, std::try_to_lock );
      if( locker.owns_lock() ) {
        if( displayCommands->size() == 0 ) {
          source.swap( displayCommands );
        }
      }
    }
    void CommandBus::consume( std::unique_ptr< Display::CommandList >& destination ) {
      // Keep this critical section short!
      std::unique_lock< std::mutex > locker( displayMutex, std::try_to_lock );
      if( locker.owns_lock() ) {
        // Okay to do the swap!
        destination.swap( displayCommands );
      }
    }
  }
}
