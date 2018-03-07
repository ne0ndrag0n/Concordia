#include "graphics/userinterface/propertylist.hpp"
#include <glm/glm.hpp>

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {

      const PropertyList _default( {
        { "left", 0 },
        { "top", 0 },
        { "width", 0 },
        { "height", 0 },
        { "padding", 0 },
        { "placement", Placement::FLOW },
        { "gravity", Gravity::LEFT },
        { "layout-weight", 0 },
        { "vertical-orientation", Orientation::TOP },
        { "horizontal-orientation", Orientation::LEFT },
        { "background-color", glm::uvec4{ 255, 255, 255, 255 } },
        { "color", glm::uvec4{ 0, 0, 0, 255 } },
        { "font", "roboto" },
        { "font-size", 16.0 },
        { "font-bold", false },
        { "font-italic", false },
        { "font-underline", false },
        { "font-strikethrough", false },
        { "local-z-order", 0 },
        { "visible", true }
      } );

      const PropertyList& PropertyList::rootPropertyList = _default;

    }
  }
}
