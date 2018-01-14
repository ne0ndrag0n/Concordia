#ifndef BB_STATE
#define BB_STATE

namespace BlueBear {
  class Application;

  namespace State {

    class State {
    protected:
      Application& application;

    public:
      State( Application& application );
      virtual ~State() = default;

      virtual void update() = 0;
    };

  }
}

#endif
