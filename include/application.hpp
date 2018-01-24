#ifndef BB_APPLICATION
#define BB_APPLICATION

#include "state/state.hpp"
#include "device/display/display.hpp"
#include "device/input/input.hpp"
#include <memory>

namespace BlueBear {

  class Application {
    std::unique_ptr< State::State > currentState;
    Device::Display::Display display;
    Device::Input::Input input;

  public:
    Application();
    virtual ~Application() = default;

    void close();
    void setupMainState();

    Device::Display::Display& getDisplayDevice();
    Device::Input::Input& getInputDevice();

    int run();
  };

}

#endif
