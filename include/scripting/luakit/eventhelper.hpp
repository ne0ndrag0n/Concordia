#ifndef NEW_EVENT_HELPER
#define NEW_EVENT_HELPER

#include "containers/reusableobjectvector.hpp"
#include "scripting/luakit/eventbridge.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <sol.hpp>
#include <unordered_map>
#include <string>

namespace BlueBear::Device::Input { class Metadata; class Input; }
namespace BlueBear::Scripting { class CoreEngine; }
namespace BlueBear::Scripting::LuaKit {

  class EventHelper {
    CoreEngine& engine;
    EventBridge bridge;
    std::unordered_map< std::string, Containers::ReusableObjectVector< sol::function > > keyEvents;

    void submitLuaContributions( sol::state& lua );
    int registerKey( const std::string& key, sol::function f );
    void unregisterKey( const std::string& key, int handle );
    void onKeyDown( Device::Input::Metadata event );

  public:
    EventHelper( CoreEngine& engine );
    ~EventHelper();

    void connectInputDevice( Device::Input::Input& inputDevice );
  };

}

#endif
