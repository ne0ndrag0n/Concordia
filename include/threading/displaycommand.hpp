#ifndef DISPLAYCOMMAND
#define DISPLAYCOMMAND

#include <memory>
#include <list>
#include <glm/glm.hpp>

namespace BlueBear {
  namespace Graphics {
    class Display;
  }

  namespace Scripting {
    class Lot;
  }

  namespace Threading {
    namespace Display {

      class Command {
        public:
          virtual void execute( Graphics::Display& instance ) = 0;
      };

      class NewEntityCommand : public Command {
        glm::vec3 position;
        public:
          void execute( Graphics::Display& instance );
      };

      class SendInfrastructureCommand : public Command {
        Scripting::Lot& lot;
        public:
          SendInfrastructureCommand( Scripting::Lot& lot );
          void execute( Graphics::Display& instance );
      };

      using CommandList = std::list< std::unique_ptr< Command > >;
    }

  }
}


#endif
