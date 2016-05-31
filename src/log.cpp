#include "log.hpp"
#include "configmanager.hpp"
#include <string>
#include <map>
#include <iomanip>
#include <ctime>
#include <fstream>

namespace BlueBear {

  Log::Log() {
    // The ConfigManager can now never ever log anything
    minimumReportableLevel = ( LogLevel )ConfigManager::getInstance().getIntValue( "min_log_level" );
    logFile.open( ConfigManager::getInstance().getValue( "logfile_path" ), std::ios_base::app );
    logFile << std::endl;
  }

  std::map< Log::LogLevel, std::string > Log::Colors = {
    { LogLevel::DEBUG, std::string( Log::ANSI_GREEN ) },
    { LogLevel::INFO, std::string( Log::ANSI_BLUE ) },
    { LogLevel::WARN, std::string( Log::ANSI_YELLOW ) },
    { LogLevel::ERROR, std::string( Log::ANSI_RED ) }
  };

  std::map< Log::LogLevel, std::string > Log::StringTypes = {
    { LogLevel::DEBUG, "d" },
    { LogLevel::INFO, "i" },
    { LogLevel::WARN, "w" },
    { LogLevel::ERROR, "e" }
  };

  void Log::out( const LogMessage& message ) {
    messages.push_back( message );

    if( message.level >= minimumReportableLevel ) {
      // sucks
      if( mode == LogMode::CONSOLE || mode == LogMode::BOTH ) {
        outToConsole( message );
      }
      if( mode == LogMode::FILE || mode == LogMode::BOTH ) {
        outToFile( message );
      }
    }
  }

  void Log::outToConsole( const LogMessage& message ) {
    auto time = std::time( nullptr );
    auto localtime = *std::localtime( &time );

    //(i)[EventManager] 2016-01-01 23:59:59: Event Noted
    // LOOK AT THIS FUCKING DISASTER!
    #ifdef _WIN32
      std::cout << "(" << Log::StringTypes[ message.level ] << ") " << std::put_time( &localtime, "%Y-%m-%d %H:%M:%S: " ) << "[" << message.tag << "] " << message.message << std::endl;
    #else
      std::cout << Log::Colors[ message.level ] << "(" << Log::StringTypes[ message.level ] << ") " << std::put_time( &localtime, "%Y-%m-%d %H:%M:%S: " ) << Log::ANSI_RESET << "[" << message.tag << "] " << message.message << std::endl;
    #endif
  }

  void Log::outToFile( const LogMessage& message ) {
    auto time = std::time( nullptr );
    auto localtime = *std::localtime( &time );

    logFile << "(" << Log::StringTypes[ message.level ] << ") " << std::put_time( &localtime, "%Y-%m-%d %H:%M:%S: " ) << "[" << message.tag << "] " << message.message << std::endl;
  }

  void Log::debug( const std::string& tag, const std::string& message ) {
    out( LogMessage { tag, message, LogLevel::DEBUG } );
  }

  void Log::info( const std::string& tag, const std::string& message ) {
    out( LogMessage { tag, message, LogLevel::INFO } );
  }

  void Log::warn( const std::string& tag, const std::string& message ) {
    out( LogMessage { tag, message, LogLevel::WARN } );
  }

  void Log::error( const std::string& tag, const std::string& message ) {
    out( LogMessage { tag, message, LogLevel::ERROR } );
  }
}
