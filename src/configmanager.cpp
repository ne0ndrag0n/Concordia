#include "configmanager.hpp"

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <jsoncpp/json/json.h>
#include <SFML/Window/Keyboard.hpp>
#include <fstream>

namespace BlueBear {

  /**
   * Clumsily define the defaults application-wide in this constructor.
   * Is there a way to do this in bulk using C++11 uniform initialisation?
   */
  ConfigManager::ConfigManager() {
    configRoot[ "fps_overview" ] = 30;
    configRoot[ "vsync_limiter_overview" ] = false;
    configRoot[ "min_log_level" ] = 0;
    configRoot[ "logfile_path" ] = "bluebear.log";
    configRoot[ "logger_mode" ] = 0;
    configRoot[ "viewport_x" ] = 1024;
    configRoot[ "viewport_y" ] = 768;
    configRoot[ "current_locale" ] = "en_US";
    configRoot[ "key_switch_perspective" ] = sf::Keyboard::P;
    configRoot[ "key_move_up" ] = sf::Keyboard::Up;
    configRoot[ "key_move_down" ] = sf::Keyboard::Down;
    configRoot[ "key_move_left" ] = sf::Keyboard::Left;
    configRoot[ "key_move_right" ] = sf::Keyboard::Right;
    configRoot[ "key_terminal" ] = sf::Keyboard::Tilde;
    configRoot[ "key_pause" ] = sf::Keyboard::Num1;
    configRoot[ "key_play" ] = sf::Keyboard::Num2;
    configRoot[ "key_play_fast" ] = sf::Keyboard::Num3;
    configRoot[ "key_rotate_right" ] = sf::Keyboard::E;
    configRoot[ "key_rotate_left" ] = sf::Keyboard::Q;
    configRoot[ "key_zoom_in" ] = sf::Keyboard::Add;
    configRoot[ "key_zoom_out" ] = sf::Keyboard::Subtract;
    configRoot[ "disable_image_cache" ] = false;
    configRoot[ "disable_texture_cache" ] = false;
    configRoot[ "ui_theme" ] = "system/ui/default.theme";
    configRoot[ "max_ingame_terminal_scrollback" ] = 100;
    configRoot[ "hud_zorder_max" ] = 1000;

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

        // Store every value as a string because strong typing is a PITA
        configRoot[ key.asString() ] = value;
      }
    }
  }

  void ConfigManager::each( std::function< void( std::string, Json::Value& ) > func ) {
    for( Json::Value::iterator jsonIterator = configRoot.begin(); jsonIterator != configRoot.end(); ++jsonIterator ) {
      func( jsonIterator.key().asString(), *jsonIterator );
    }
  }

  std::string ConfigManager::getValue( const std::string& key ) {
    return configRoot[ key ].asString();
  }

  int ConfigManager::getIntValue( const std::string& key ) {
    return configRoot[ key ].asInt();
  }

  bool ConfigManager::getBoolValue( const std::string& key ) {
    return configRoot[ key ].asBool();
  }

  /**
   * Get a value. This always returns a string. If you want an int, use to_number
   * inside lua.
   */
  int ConfigManager::lua_getValue( lua_State* L ) {
    // Linker errors when using the singleton directly, I don't know why.
    ConfigManager& self = *( ( ConfigManager* )lua_touserdata( L, lua_upvalueindex( 1 ) ) );

    if( lua_isstring( L, -1 ) ) {
      std::string key = lua_tostring( L, -1 );
      std::string result = self.getValue( key );

      if( result == "" ) {
        // Looked up an invalid key
        lua_pushnil( L );
      } else {
        lua_pushstring( L, result.c_str() );
      }

      return 1;
    } else {
      return luaL_error( L, "ConfigManager: Failed to provide a string key." );
    }
  }
}
