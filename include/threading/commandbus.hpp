#ifndef COMMANDBUS
#define COMMANDBUS

#include "threading/displaycommand.hpp"
#include "threading/enginecommand.hpp"
#include <memory>
#include <vector>
#include <mutex>

namespace BlueBear {
  namespace Threading {

    class CommandBus {

      public:
        std::mutex displayMutex;
        std::unique_ptr< Display::CommandList > displayCommands;
        std::mutex engineMutex;
        std::unique_ptr< Engine::CommandList > engineCommands;

        CommandBus();

        /**
         * Producer/consumer pair for Display::CommandList
         */
        bool attemptProduce( Display::CommandList& source );
        bool attemptConsume( std::unique_ptr< Display::CommandList >& destination );
        void produce( Display::CommandList& source );
        void consume( std::unique_ptr< Display::CommandList >& destination );

        /**
         * Producer/consumer pair for Engine::CommandList
         */
        bool attemptProduce( Engine::CommandList& source );
        bool attemptConsume( std::unique_ptr< Engine::CommandList >& destination );
        void produce( Engine::CommandList& source );
        void consume( std::unique_ptr< Engine::CommandList >& destination );
    };

  }
}


#endif
