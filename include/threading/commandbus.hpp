#ifndef COMMANDBUS
#define COMMANDBUS

#include "threading/displaycommand.hpp"
#include <memory>
#include <vector>
#include <mutex>

namespace BlueBear {
  namespace Threading {

    class CommandBus {

      public:
        std::mutex displayMutex;
        std::unique_ptr< Display::CommandList > displayCommands;

        CommandBus();

        bool attemptProduce( Display::CommandList& source );
        bool attemptConsume( std::unique_ptr< Display::CommandList >& destination );
    };

  }
}


#endif
