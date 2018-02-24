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
        { "min-width", 10 },
        { "min-height", 10 },
        { "max-width", -1 },
        { "max-height", -1 },
        { "placement", "grid" },
        { "background-mode", "color" },
        { "background-pattern", "" },
        { "background-color", glm::uvec4{ 255, 255, 255, 255 } },
        { "local-z-order", 0 },
        { "visible", true }
      } );

      const PropertyList& PropertyList::rootPropertyList = _default;

    }
  }
}
