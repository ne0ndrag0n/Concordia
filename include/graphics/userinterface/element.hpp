#ifndef NEW_GUI_ELEMENT
#define NEW_GUI_ELEMENT

#include "graphics/userinterface/drawable.hpp"
#include "graphics/userinterface/propertylist.hpp"
#include <sol.hpp>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <optional>

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
        std::weak_ptr< Element > parent;
        const std::string& tag;
        std::string id;
        std::vector< std::string > classes;
        PropertyList localStyle;
        std::optional< Drawable > drawable;
        std::vector< std::shared_ptr< Element > > children;
        std::unordered_map< std::string, sol::function > events;

        Element( const std::string& tag, const std::string& id, const std::vector< std::string >& classes );
        Element( const Element& other );

      public:
        static std::shared_ptr< Element > create( const std::string& tag, const std::string& id, const std::vector< std::string >& classes );
        std::shared_ptr< Element > copy();

        void reflow( Vector::Renderer& vectorRenderer );
        void draw();
      };

    }
  }
}



#endif
