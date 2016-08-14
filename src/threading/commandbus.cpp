#include "threading/commandbus.hpp"
#include "threading/displaycommand.hpp"
#include "threading/enginecommand.hpp"
#include <memory>
#include <mutex>
#include <utility>

namespace BlueBear {
  namespace Threading {

    CommandBus::CommandBus() {
      displayCommands = std::make_unique< Display::CommandList >();
      engineCommands = std::make_unique< Engine::CommandList >();
    }

    bool CommandBus::attemptProduce( Display::CommandList& source ) {
      std::unique_lock< std::mutex > locker( displayMutex, std::try_to_lock );
      bool successful = locker.owns_lock();
      if( successful ) {
        displayCommands->splice( displayCommands->end(), source );
      }

      return successful;
    }

    bool CommandBus::attemptConsume( std::unique_ptr< Display::CommandList >& destination ) {
      // Keep this critical section short!
      std::unique_lock< std::mutex > locker( displayMutex, std::try_to_lock );
      bool successful = locker.owns_lock();
      if( successful ) {
        displayCommands.swap( destination );
      }

      return successful;
    }

    void CommandBus::produce( Display::CommandList& source ) {
      std::unique_lock< std::mutex > locker( displayMutex );
      displayCommands->splice( displayCommands->end(), source );
    }

    void CommandBus::consume( std::unique_ptr< Display::CommandList >& destination ) {
      std::unique_lock< std::mutex > locker( displayMutex );
      displayCommands.swap( destination );
    }

    bool CommandBus::attemptProduce( Engine::CommandList& source ) {
      std::unique_lock< std::mutex > locker( engineMutex, std::try_to_lock );
      bool successful = locker.owns_lock();
      if( successful ) {
        engineCommands->splice( engineCommands->end(), source );
      }

      return successful;
    }
    bool CommandBus::attemptConsume( std::unique_ptr< Engine::CommandList >& destination ) {
      std::unique_lock< std::mutex > locker( engineMutex, std::try_to_lock );
      bool successful = locker.owns_lock();
      if( successful ) {
        engineCommands.swap( destination );
      }
      return successful;
    }

    void CommandBus::produce( Engine::CommandList& source ) {
      std::unique_lock< std::mutex > locker( engineMutex );
      engineCommands->splice( engineCommands->end(), source );
    }

    void CommandBus::consume( std::unique_ptr< Engine::CommandList >& destination ) {
      std::unique_lock< std::mutex > locker( engineMutex );
      engineCommands.swap( destination );
    }
  }
}
