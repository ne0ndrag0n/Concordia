#ifndef NEW_MODPACK_LOADER
#define NEW_MODPACK_LOADER

#include "bbtypes.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <sol.hpp>
#include <string>
#include <map>

namespace BlueBear::Scripting::LuaKit {

  class ModpackLoader {
    static constexpr const char* MODPACK_MAIN_SCRIPT = "main.lua";

    sol::state& lua;
    std::string currentModpackDirectory;
    std::map< std::string, ModpackStatus > loadedModpacks;

    bool loadModpack( const std::string& name );

  public:
    ModpackLoader( sol::state& lua, const std::string& currentModpackDirectory );
    ~ModpackLoader();

    void load();
  };

}

#endif
