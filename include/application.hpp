#ifndef BB_APPLICATION
#define BB_APPLICATION

#include "state/state.hpp"
#include <memory>

namespace BlueBear {

  class Application {
    std::unique_ptr< State::State > currentState;

  public:
    Application();
    virtual ~Application() = default;

    void close();
    void setupMainState();

    int run();
  };

}

#endif
