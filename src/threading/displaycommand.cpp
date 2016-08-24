#include "threading/displaycommand.hpp"
#include "graphics/display.hpp"
#include "scripting/lot.hpp"
#include "log.hpp"

namespace BlueBear {
  namespace Threading {
    namespace Display {

      void NewEntityCommand::execute( Graphics::Display& instance ) {
        Log::getInstance().info( "NewEntityCommand", "Called registerNewEntity, hang in there..." );
      }

      SendInfrastructureCommand::SendInfrastructureCommand( Scripting::Lot& lot ) : lot( lot ) {}
      void SendInfrastructureCommand::execute( Graphics::Display& instance ) {
        instance.loadInfrastructure( lot );
      }
    }
  }
}
