#ifndef LOGMANAGER
#define LOGMANAGER

#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <mutex>

namespace BlueBear {

  /**
   * Collection of methods that handle logging in a cross-platform manner.
   */
  class Log {

    private:
      static constexpr const char* ANSI_RESET = "\033[0m";
      static constexpr const char* ANSI_RED = "\033[31m";
      static constexpr const char* ANSI_YELLOW = "\033[33m";
      static constexpr const char* ANSI_BLUE = "\033[36m";
      static constexpr const char* ANSI_GREEN = "\033[32m";

      enum class LogMode : int { BOTH, CONSOLE, FILE };
      enum class LogLevel : int { LEVEL_DEBUG, LEVEL_INFO, LEVEL_WARN, LEVEL_ERROR };
      static std::map< LogLevel, std::string > Colors;
      static std::map< LogLevel, std::string > StringTypes;

      struct LogMessage {
        std::string tag;
        std::string message;
        LogLevel level;
      };

      std::mutex mutex;
      std::ofstream logFile;
      std::vector< LogMessage > messages;
      LogLevel minimumReportableLevel;
      LogMode mode;

      Log();
      Log( Log const& );
      void operator=( Log const& );
      void out( const LogMessage& message );
      void outToConsole( const std::string& text );
      void outToFile( const std::string& text );
      std::string messageToString( const LogMessage& message, bool accent );

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
