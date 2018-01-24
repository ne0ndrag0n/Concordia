#ifndef WORLD_DISPLAY_ADAPTER
#define WORLD_DISPLAY_ADAPTER

#include "device/display/adapter/adapter.hpp"
#include "device/display/adapter/component/guicomponent.hpp"
#include "device/display/adapter/component/worldrenderer.hpp"

namespace BlueBear {
  namespace Graphics {
    class Camera;
  }

  namespace Device {
    namespace Display {
      namespace Adapter {

        class WorldAdapter : public Adapter, public Component::GuiComponent, public Component::WorldRenderer {

        public:
          WorldAdapter( Display& display );

          void nextFrame() override;
        };

      }
    }
  }
}

#endif
