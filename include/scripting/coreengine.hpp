#ifndef NEW_ENGINE
#define NEW_ENGINE

#include "containers/reusableobjectvector.hpp"
#include "containers/visitor.hpp"
#include "eventmanager.hpp"
#include "state/substate.hpp"
#include <sol.hpp>
#include <vector>
#include <functional>
#include <variant>

namespace BlueBear::Scripting {

  class CoreEngine : public State::Substate {
  public:
    using Callback = std::variant< sol::function, std::function< void() > >;

  private:
    static constexpr const char* USER_MODPACK_DIRECTORY = "modpacks/user/";
    static constexpr const char* SYSTEM_MODPACK_DIRECTORY = "modpacks/system/";

    sol::state lua;
    Containers::ReusableObjectVector< std::pair< int, Callback > > queuedCallbacks;

    void setupCoreEnvironment();

    sol::function bind( sol::function f, sol::variadic_args args );
    static double secondsToTicks( double seconds );

  public:
    // TODO: Refactor to hold LUA_STATE_READY
    static BasicEvent< void* > LUA_STATE_CLOSE;

    CoreEngine( State::State& state );
    ~CoreEngine();

    int setTimeout( double interval, Callback f );
    void cancelTimeout( int index );
    void loadModpacks();
    void broadcastReadyEvent();
    bool update() override;
  };

}

#endif
