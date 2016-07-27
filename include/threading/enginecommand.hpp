#ifndef ENGINECOMMAND
#define ENGINECOMMAND

#include <list>
#include <memory>

namespace BlueBear {
  namespace Scripting {
    class Engine;
  }

  namespace Graphics {
    class Instance;
  }

  namespace Threading {
    namespace Engine {

      class Command {
        public:
          virtual void execute( Scripting::Engine& instance ) = 0;
      };

      class RegisterInstance : public Command {
        unsigned int instanceId;

        public:
          RegisterInstance( unsigned int instanceId );
          void execute( Scripting::Engine& instance );
      };

      using CommandList = std::list< std::unique_ptr< Command > >;
    }
  }
}

#endif
