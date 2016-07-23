#ifndef COMMANDBUS
#define COMMANDBUS

#include "graphics/display.hpp"
#include <memory>
#include <vector>
#include <mutex>

namespace BlueBear {
  namespace Threading {

    class CommandBus {

      public:
        std::mutex displayMutex;
        std::unique_ptr< Graphics::Display::CommandList > displayCommands;

        CommandBus();

        void produce( std::unique_ptr< Graphics::Display::CommandList >& source );
        void consume( std::unique_ptr< Graphics::Display::CommandList >& destination );
    };

  }
}


#endif
