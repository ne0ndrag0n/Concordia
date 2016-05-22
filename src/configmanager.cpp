#include "configmanager.hpp"

#include "json/json.h"
#include <iostream>
#include <fstream>

namespace BlueBear {

  /**
   * Clumsily define the defaults application-wide in this constructor.
   * Is there a way to do this in bulk using C++11 uniform initialisation?
   */
  ConfigManager::ConfigManager() {
    configRoot[ "ticks_per_second" ] = 30;

    // Load settings.json from file
    std::ifstream settingsFile( SETTINGS_PATH );
    Json::Value settingsJSON;
    Json::Reader reader;

    if( reader.parse( settingsFile, settingsJSON ) ) {
      // Override defaults
      // iterators - barf
      for( Json::Value::iterator jsonIterator = settingsJSON.begin(); jsonIterator != settingsJSON.end(); ++jsonIterator ) {
        // BARF
        Json::Value key = jsonIterator.key();
        Json::Value value = *jsonIterator;

        std::cout << "ConfigManager: Overriding settings default [" << key << "] " << value << std::endl;

        // Store every value as a string because strong typing is a PITA
        configRoot[ key.asString() ] = value;
      }
    }
  }

  std::string ConfigManager::getValue( const std::string& key ) {
    return configRoot[ key ].asString();
  }

  int ConfigManager::getIntValue( const std::string& key ) {
    return configRoot[ key ].asInt();
  }
}
