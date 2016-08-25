#ifndef COMMANDBUS
#define COMMANDBUS

#include "graphics/display.hpp"
#include "scripting/engine.hpp"
#include <memory>
#include <vector>
#include <mutex>

namespace BlueBear {
  namespace Threading {

    class CommandBus {

      public:
        std::mutex displayMutex;
        std::unique_ptr< Graphics::Display::CommandList > displayCommands;
        std::mutex engineMutex;
        std::unique_ptr< Scripting::Engine::CommandList > engineCommands;

        CommandBus();

        /**
         * Producer/consumer pair for Display::CommandList
         */
        bool attemptProduce( Graphics::Display::CommandList& source );
        bool attemptConsume( Graphics::Display::CommandList& destination );
        void produce( Graphics::Display::CommandList& source );
        void consume( Graphics::Display::CommandList& destination );

        /**
         * Producer/consumer pair for Engine::CommandList
         */
        bool attemptProduce( Scripting::Engine::CommandList& source );
        bool attemptConsume( Scripting::Engine::CommandList& destination );
        void produce( Scripting::Engine::CommandList& source );
        void consume( Scripting::Engine::CommandList& destination );
    };

  }
}


#endif
