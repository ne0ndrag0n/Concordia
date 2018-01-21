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

      template < typename Class >
      Class& as() {
        return dynamic_cast< Class& >( *this );
      }

      Application& getApplication();

      virtual void update() = 0;
    };

  }
}

#endif
