#ifndef DEVICE_DISPLAY_ADAPTER
#define DEVICE_DISPLAY_ADAPTER

namespace BlueBear {
  namespace Device {
    namespace Display {
      class Display;

      namespace Adapter {

        class Adapter {
          Display& display;

        public:
          Adapter( Display& display );

          template < typename Class >
          Class& as() {
            return dynamic_cast< Class& >( *this );
          }

          virtual void nextFrame() = 0;
        };

      }
    }
  }
}

#endif
