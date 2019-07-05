#ifndef CONFIGMANAGER
#define CONFIGMANAGER

#include <jsoncpp/json/json.h>
#include <sol.hpp>
#include <string>
#include <map>
#include <functional>

namespace BlueBear {
  class ConfigManager {
    public:
      Json::Value configRoot;

      static ConfigManager& getInstance() {
        static ConfigManager instance;
        return instance;
      }

      std::string getValue( const std::string& key );
      int getIntValue( const std::string& key );
      bool getBoolValue( const std::string& key );

      void each( std::function< void( std::string, Json::Value& ) > func );

      void submitLuaContributions( sol::state& lua );

    private:
      static constexpr const char* SETTINGS_PATH = "settings.json";

      ConfigManager();
      ConfigManager( ConfigManager const& );
      void operator=( ConfigManager const& );
  };
}


#endif
