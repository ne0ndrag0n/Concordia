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
        std::unique_ptr< Display::CommandSeries > displayCommands;

        CommandBus();

        void produce( std::unique_ptr< Display::CommandList >& source );
        void consume( std::unique_ptr< Display::CommandSeries >& destination );
    };

  }
}


#endif
