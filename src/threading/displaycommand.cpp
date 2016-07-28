#include "threading/displaycommand.hpp"
#include "graphics/display.hpp"
#include "log.hpp"

namespace BlueBear {
  namespace Threading {
    namespace Display {

      void NewEntityCommand::execute( Graphics::Display& instance ) {
        instance.registerNewEntity();

        Log::getInstance().info( "NewEntityCommand", "Called registerNewEntity, hang in there..." );
      }

    }
  }
}
