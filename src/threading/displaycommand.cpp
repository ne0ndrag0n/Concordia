#include "threading/displaycommand.hpp"
#include "graphics/display.hpp"
#include "log.hpp"

namespace BlueBear {
  namespace Threading {
    namespace Display {

      void NewEntityCommand::execute( Graphics::Display& instance ) {
        Log::getInstance().debug( "NewEntityCommand", "Successfully ran a task within display." );
      }

    }
  }
}
