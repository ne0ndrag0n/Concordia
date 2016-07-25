#ifndef DISPLAYCOMMAND
#define DISPLAYCOMMAND

#include <memory>
#include <list>
#include <glm/glm.hpp>

namespace BlueBear {
  namespace Graphics {
    class Display;
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

      using CommandList = std::list< std::unique_ptr< Command > >;
    }

  }
}


#endif
