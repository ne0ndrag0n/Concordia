#include "localemanager.hpp"
#include <jsoncpp/json/json.h>
#include <fstream>
#include "configmanager.hpp"
#include "log.hpp"

namespace BlueBear {

  LocaleManager::LocaleManager() {
    setCurrentLocale( ConfigManager::getInstance().getValue( "current_locale" ) );
  }

  /**
   * Set the current locale
   */
  void LocaleManager::setCurrentLocale( const std::string& locale ) {
    localeTable.clear();

    // Load the localepack
    std::string localePath = LOCALE_PATH + locale + ".json";
    std::ifstream localeFile( localePath );

    if( localeFile.is_open() && localeFile.good() ) {
      Json::Reader reader;
      if( !reader.parse( localeFile, localeTable ) ) {
        Log::getInstance().error( "LocaleManager", "Failed to load path: " + localePath );
      } else {
        currentLocale = locale;
        Log::getInstance().debug( "LocaleManager", "Loaded locale for " + currentLocale + " successfully." );
      }
    }
  }

  /**
   * Get a string
   */
  std::string LocaleManager::getString( const std::string& key ) {
    if( localeTable.isMember( key ) ) {
      return localeTable[ key ].asString();
    } else {
      Log::getInstance().warn( "LocaleManager", "No key for localisation string " + key + " in locale " + currentLocale );
      return "undefined";
    }
  }

}
