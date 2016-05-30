#include "log.hpp"
#include <string>

namespace BlueBear {

  void Log::out( const LogMessage& message ) {
    messages.push_back( message );

    // sucks
    if( mode == LogMode::CONSOLE || mode == LogMode::BOTH ) {
      outToConsole( message );
    }
    if( mode == LogMode::FILE || mode == LogMode::BOTH ) {
      outToFile( message );
    }
  }

  void Log::outToConsole( const LogMessage& message ) {
    #ifdef _WIN32
      std::cout << "[" << message.tag << "] " << message.message << std::endl;
    #else
      std::cout << "[" << message.tag << "] " << message.message << std::endl;
    #endif
  }

  void Log::outToFile( const LogMessage& message ) {
    // STUB !!
  }

  void Log::debug( const std::string& tag, const std::string& message ) {
    out( LogMessage { tag, message, LogLevel::DEBUG } );
  }


}
