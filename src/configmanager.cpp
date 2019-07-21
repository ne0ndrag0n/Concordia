#include "configmanager.hpp"
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
    configRoot[ "floor_texture_size" ] = 48;
    configRoot[ "wall_texture_size" ] = 48;
    configRoot[ "sector_resolution" ] = 1;
    configRoot[ "bounding_volume_method" ] = "aabb";
    configRoot[ "shader_max_diffuse_textures" ] = 4;
    configRoot[ "shader_max_specular_textures" ] = 4;
    configRoot[ "shader_max_lights" ] = 16;
    configRoot[ "shader_max_rooms" ] = 16;
    configRoot[ "shader_room_map_min_width" ] = 1000;
    configRoot[ "shader_room_map_min_height"] = 1000;
    configRoot[ "wall_cutaway_animation_speed" ] = 1000;
    configRoot[ "shader_grid_selectable_tiles" ] = 32;
    configRoot[ "shader_grid_line_size" ] = 25;
    configRoot[ "debug_console_trim" ] = 50;
    configRoot[ "camera_scroll_snap" ] = 33;

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

  void ConfigManager::submitLuaContributions( sol::state& lua ) {
    sol::table config = lua[ "bluebear" ][ "config" ] = lua.create_table();

    config.set_function( "get_string_value", &ConfigManager::getValue, this );
    config.set_function( "get_int_value", &ConfigManager::getIntValue, this );
    config.set_function( "get_bool_value", &ConfigManager::getBoolValue, this );
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
}
