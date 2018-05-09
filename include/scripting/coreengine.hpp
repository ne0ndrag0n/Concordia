#ifndef NEW_ENGINE
#define NEW_ENGINE

#include "containers/reusableobjectvector.hpp"
#include "containers/visitor.hpp"
#include "state/substate.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <sol.hpp>
#include <vector>
#include <functional>
#include <variant>

namespace BlueBear::Scripting {

  class CoreEngine : public State::Substate {
    static constexpr const char* USER_MODPACK_DIRECTORY = "modpacks/user/";
    static constexpr const char* SYSTEM_MODPACK_DIRECTORY = "modpacks/system/";

    sol::state lua;
    Containers::ReusableObjectVector< std::pair< int, std::variant< sol::function, std::function< void() > > > > queuedCallbacks;

    void setupCoreEnvironment();

    sol::function bind( sol::function f, sol::variadic_args args );
    static double secondsToTicks( double seconds );

  public:
    CoreEngine( State::State& state );

    int setTimeout( double interval, sol::function f );
    void loadModpacks();
    void broadcastReadyEvent();
    bool update() override;
  };

}

#endif
