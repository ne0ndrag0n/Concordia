#include "threading/commandbus.hpp"
#include <memory>

namespace BlueBear {
  namespace Threading {

    CommandBus::CommandBus() {
      displayCommands = std::make_unique< Graphics::Display::CommandList >();
    }


  }
}
