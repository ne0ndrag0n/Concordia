#ifndef CONFIGMANAGER
#define CONFIGMANAGER

#include "json/json.h"
#include <string>
#include <map>

namespace BlueBear {
  class ConfigManager {
    public:
      static ConfigManager& getInstance() {
        static ConfigManager instance;
        return instance;
      }

      std::string getValue( const std::string& key );
      int getIntValue( const std::string& key );
    private:
      static constexpr const char* SETTINGS_PATH = "settings.json";

      ConfigManager();
      ConfigManager( ConfigManager const& );
      void operator=( ConfigManager const& );
      Json::Value configRoot;
  };
}


#endif
