#ifndef BB_SUBSTATE
#define BB_SUBSTATE

namespace BlueBear {
  namespace State {
    class State;

    class Substate {
    protected:
      State& state;

    public:
      Substate( State& state );
      virtual ~Substate() = default;

      virtual bool update() = 0;
    };

  }
}

#endif
