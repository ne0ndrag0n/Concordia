#ifndef MODEL_LOADER
#define MODEL_LOADER

#include "graphics/model.hpp"
#include <map>
#include <string>
#include <memory>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

namespace BlueBear {
  namespace Graphics {
    class Instance;
    class Shader;

    using ModelLoader = std::map< std::string, std::shared_ptr< Model > >;

    class ModelLoaderHelper {
    public:
      static int lua_createModelLoader( lua_State* L );
      static int lua_loadModel( lua_State* L );
      static int lua_getInstance( lua_State* L );
      static int lua_gc( lua_State* L );
    };

    class LuaInstanceHelper {
    public:
      std::shared_ptr< Shader > shader;
      std::shared_ptr< Instance > instance;

      static int lua_getAnimList( lua_State* L );
      static int lua_getAnimation( lua_State* L );
      static int lua_setAnimation( lua_State* L );
      static int lua_pauseAnimation( lua_State* L );
      static int lua_isAnimationPaused( lua_State* L );
      static int lua_setAnimationFrame( lua_State* L );
      static int lua_getAnimationDuration( lua_State* L );
      static int lua_gc( lua_State* L );
    };
  }
}

#endif
