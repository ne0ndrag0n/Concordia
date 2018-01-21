#include "state/state.hpp"

namespace BlueBear {
  namespace State {

    State::State( Application& application ) : application( application ) {}

    Application& State::getApplication() {
      return application;
    }

  }
}
