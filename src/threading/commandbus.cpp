#include "threading/commandbus.hpp"
#include "threading/displaycommand.hpp"
#include <memory>
#include <mutex>
#include <utility>

namespace BlueBear {
  namespace Threading {

    CommandBus::CommandBus() {
      displayCommands = std::make_unique< Display::CommandList >();
    }

    void CommandBus::attemptProduce( Display::CommandList& source ) {
      std::unique_lock< std::mutex > locker( displayMutex, std::try_to_lock );
      if( locker.owns_lock() ) {
        displayCommands->splice( displayCommands->end(), source );
      }
    }
    void CommandBus::attemptConsume( std::unique_ptr< Display::CommandList >& destination ) {
      // Keep this critical section short!
      std::unique_lock< std::mutex > locker( displayMutex, std::try_to_lock );
      if( locker.owns_lock() ) {
        displayCommands.swap( destination );
      }
    }
  }
}
