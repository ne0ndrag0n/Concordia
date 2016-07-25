#include "threading/commandbus.hpp"
#include "threading/displaycommand.hpp"
#include <memory>
#include <mutex>
#include <utility>

namespace BlueBear {
  namespace Threading {

    CommandBus::CommandBus() {
      displayCommands = std::make_unique< Display::CommandSeries >();
    }

    void CommandBus::produce( std::unique_ptr< Display::CommandList >& source ) {
      std::unique_lock< std::mutex > locker( displayMutex, std::try_to_lock );
      if( locker.owns_lock() ) {
        // Plop the given Display::CommandList onto the pile (CommandSeries)
        displayCommands->push_back( std::move( source ) );
      }
    }
    void CommandBus::consume( std::unique_ptr< Display::CommandSeries >& destination ) {
      // Keep this critical section short!
      std::unique_lock< std::mutex > locker( displayMutex, std::try_to_lock );
      if( locker.owns_lock() ) {
        displayCommands.swap( destination );
      }
    }
  }
}
