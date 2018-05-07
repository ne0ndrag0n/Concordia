#include "scripting/luakit/modpackloader.hpp"
#include "tools/utility.hpp"
#include "log.hpp"
#include <functional>

namespace BlueBear::Scripting::LuaKit {

  ModpackLoader::ModpackLoader( sol::state& lua, const std::string& currentModpackDirectory ) : lua( lua ), currentModpackDirectory( currentModpackDirectory ) {
    sol::table engine = lua[ "bluebear" ][ "engine" ];
    engine.set_function( "require_modpack", [ & ]( sol::this_state state, const std::string& path ) {
      if( !loadModpack( path ) ) {
        luaL_error( state.lua_state(), "Failed to load modpack: %s", path.c_str() );
      }
    } );
  }

  ModpackLoader::~ModpackLoader() {
    lua[ "bluebear" ][ "engine" ][ "require_modpack" ] = []() {};
  }

  bool ModpackLoader::loadModpack( const std::string& name ) {
    lua_State* L = lua.lua_state();

    // If this modpack is LOADING, don't load it twice! This is a circular dependency; a modpack being imported by another modpack called
    // to load the first modpack (which was still LOADING)! Fail immediately.
    // Fail immediately if it already failed (don't waste time loading it again)
    if( loadedModpacks[ name ] == ModpackStatus::LOADING || loadedModpacks[ name ] == ModpackStatus::LOAD_FAILED ) {
      return false;
    }

    // Don't load another modpack twice. Another script might have already loaded this modpack. Just go "uh-huh" and
    // remind the source call that this modpack was already loaded.
    if( loadedModpacks[ name ] == ModpackStatus::LOAD_SUCCESSFUL ) {
      return true;
    }

    // Assemble the fully-qualified pathname for this modpack
    std::string path = currentModpackDirectory + name;
    std::string fullPath = path + "/" + MODPACK_MAIN_SCRIPT;

    // Mark the module as LOADING - first if should catch this module if it's called again without completing
    loadedModpacks[ name ] = ModpackStatus::LOADING;

    // dofile pointed to by path
    if( luaL_loadfile( L, fullPath.c_str() ) || !lua_pushstring( L, path.c_str() ) || lua_pcall( L, 1, LUA_MULTRET, 0 ) ) {
      // Exception occurred during opening the modpack
      // Exception occurred during the integration of this modpack
      Log::getInstance().error( "Engine::loadModpack", "Failed to integrate modpack " + name + ": " + lua_tostring( L, -1 ) );
      lua_pop( L, 1 );
      loadedModpacks[ name ] = ModpackStatus::LOAD_FAILED;
      return false;
    }

    loadedModpacks[ name ] = ModpackStatus::LOAD_SUCCESSFUL;
    return true;
  }

  void ModpackLoader::load() {
    auto modpacks = Tools::Utility::getSubdirectoryList( currentModpackDirectory.c_str() );

    for( auto& modpack : modpacks ) {
      if( !loadModpack( modpack ) ) {
        return;
      }
    }
  }

}
