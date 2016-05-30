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
      enum class LogMode { CONSOLE, FILE, BOTH };
      enum class LogLevel { DEBUG, INFO, WARN, ERROR };

      struct LogMessage {
        std::string tag;
        std::string message;
        LogLevel level;
      };

      std::vector< LogMessage > messages;
      LogLevel minimumReportableLevel;
      LogMode mode;

      Log();
      Log( Log const& );
      void operator=( Log const& );
      void out( const LogMessage& message );
      void outToConsole( const LogMessage& message );
      void outToFile( const LogMessage& message );

    public:
      static Log& getInstance() {
        static Log instance;
        return instance;
      }

      void debug( const std::string& tag, const std::string& message );
      void info( const std::string& tag, const std::string& message );
      void warn( const std::string& tag, const std::string& message );
      void error( const std::string& tag, const std::string& message );
  };
}

#endif
