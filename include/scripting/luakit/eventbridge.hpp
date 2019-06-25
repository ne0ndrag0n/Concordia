#ifndef LUA_EVENT_BRIDGE
#define LUA_EVENT_BRIDGE

#include "containers/reusableobjectvector.hpp"
#include <sol.hpp>
#include <map>
#include <string>

namespace BlueBear::Scripting { class CoreEngine; }
namespace BlueBear::Scripting::LuaKit {

  class EventBridge {
    CoreEngine& coreEngine;
    std::map< std::string, Containers::ReusableObjectVector< sol::function > > registered;

  public:
    EventBridge( CoreEngine& coreEngine );
    ~EventBridge();

    void submitLuaContributions( sol::table event );

    int registerEvent( const std::string& key, sol::function f );
    void unregisterEvent( const std::string& key, int index );
    void fireEvents( const std::string& key, const std::string& arg1 );
  };

}

#endif
