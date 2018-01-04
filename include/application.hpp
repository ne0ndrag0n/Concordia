#ifndef BB_APPLICATION
#define BB_APPLICATION

#include "exceptions/genexc.hpp"
#include "scripting/engine.hpp"
#include "graphics/display.hpp"
#include <memory>

namespace BlueBear {

  class Application {
    std::unique_ptr< Scripting::Engine > engine;
    std::unique_ptr< Graphics::Display > display;

    std::unique_ptr< Scripting::Engine > getEngine();
    std::unique_ptr< Graphics::Display > getDisplay();

  public:
    EXCEPTION_TYPE( LotNotFoundException, "Lot not found!" );
    EXCEPTION_TYPE( EngineLoadFailureException, "Failed to load BlueBear!" );
    Application();
    virtual ~Application() = default;

    void setupMainState();

    int run();
  };

}

#endif
