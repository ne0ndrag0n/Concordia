#ifndef LOGMANAGER
#define LOGMANAGER

#include <iostream>
#include <string>
#include <vector>

namespace BlueBear {

  /**
   * Collection of methods that handle logging in a cross-platform manner.
   */
  class Log {

    private:
      enum class LogLevel { DEBUG, INFO, WARN, ERROR };

      class LogMessage {
        std::string tag;
        std::string message;
        LogLevel level;
      };

      std::vector< LogMessage > messages;

      Log();
      Log( Log const& );
      void operator=( Log const& );

    public:
      static Log& getInstance() {
        static Log instance;
        return instance;
      }

      void debug( std::string tag, std::string message );
      void info( std::string tag, std::string message );
      void warn( std::string tag, std::string message );
      void error( std::string tag, std::string message );
  };
}

#endif
