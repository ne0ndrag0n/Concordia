#ifndef NEW_GUI_ELEMENT
#define NEW_GUI_ELEMENT

#include "graphics/userinterface/drawable.hpp"
#include "graphics/userinterface/propertylist.hpp"
#include "log.hpp"
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace BlueBear {
  namespace Device {
    namespace Display {
      namespace Adapter {
        namespace Component {
          class GuiComponent;
        }
      }
    }
  }

  namespace Graphics {
    namespace UserInterface {

      class Element : public std::enable_shared_from_this< Element > {
      protected:
        const std::string& tag;
        std::string id;
        std::vector< std::string > classes;

        PropertyList localStyle;
        std::unique_ptr< Drawable > drawable;

        std::weak_ptr< Element > parent;
        std::vector< std::shared_ptr< Element > > children;

        Element( const std::string& tag, const std::string& id, const std::vector< std::string >& classes );
        Element( const Element& other );
        virtual ~Element();

      public:
        virtual void reflow( Device::Display::Adapter::Component::GuiComponent& manager ) = 0;
        virtual void draw() = 0;
      };

    }
  }
}



#endif
