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
        std::unique_ptr< std::vector< BlueBear::Graphics::Display::Command > > display;

        CommandBus();
    };

  }
}


#endif
