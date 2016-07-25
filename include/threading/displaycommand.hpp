#ifndef DISPLAYCOMMAND
#define DISPLAYCOMMAND

#include <memory>
#include <deque>
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

      using CommandList = std::deque< std::unique_ptr< Command > >;
      using CommandSeries = std::deque< std::unique_ptr< CommandList > >;
    }

  }
}


#endif
