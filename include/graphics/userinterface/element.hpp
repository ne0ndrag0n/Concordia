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
    namespace Vector {
      class Renderer;
    }

    namespace UserInterface {

      class Element : public std::enable_shared_from_this< Element > {
      protected:
        const std::string& tag;
        std::string id;
        std::vector< std::string > classes;

        PropertyList localStyle;
        glm::uvec2 requisition;
        glm::uvec4 allocation;
        std::unique_ptr< Drawable > drawable;

        std::weak_ptr< Element > parentWeak;
        std::vector< std::shared_ptr< Element > > children;

        Element( const std::string& tag, const std::string& id, const std::vector< std::string >& classes );
        Element( const Element& other );
        virtual ~Element();

        bool valueIsLiteral( int r );
        glm::uvec2 getAbsolutePosition();
        virtual std::vector< std::shared_ptr< Element > > getSortedElements();

      public:
        static Device::Display::Adapter::Component::GuiComponent* manager;

        virtual void positionAndSizeChildren();
        virtual void render() = 0;
        virtual void calculate() = 0;

        std::shared_ptr< Element > getParent();
        void addChild( std::shared_ptr< Element > child );
        void detach();

        PropertyList& getPropertyList();
        glm::uvec2 getRequisition();

        void setAllocation( const glm::uvec4& allocation );
        void reflow();
        void draw();
      };

    }
  }
}



#endif
