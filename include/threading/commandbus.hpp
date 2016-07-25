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

        void attemptProduce( Display::CommandList& source );
        void attemptConsume( std::unique_ptr< Display::CommandList >& destination );
    };

  }
}


#endif
