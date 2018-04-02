#ifndef NEW_GUI_ELEMENT
#define NEW_GUI_ELEMENT

#include "graphics/userinterface/drawable.hpp"
#include "graphics/userinterface/style/style.hpp"
#include "log.hpp"
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

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
        std::string tag;
        std::string id;
        std::vector< std::string > classes;

        Style::Style localStyle;
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
        virtual bool isDrawableValid();
        virtual void generateDrawable();

      public:
        static Device::Display::Adapter::Component::GuiComponent* manager;

        virtual void positionAndSizeChildren();
        virtual void render( Graphics::Vector::Renderer& renderer );
        virtual void calculate() = 0;

        const std::string& getTag() const;
        const std::string& getId() const;
        bool hasClass( const std::string& clss ) const;
        std::vector< std::shared_ptr< Element > > getChildren() const;

        std::shared_ptr< Element > getParent();
        void addChild( std::shared_ptr< Element > child );
        void detach();

        Style::Style& getPropertyList();
        glm::uvec2 getRequisition();

        void setAllocation( const glm::uvec4& allocation, bool doReflow = true );
        void reflow();
        void paint();
        void draw();
      };

    }
  }
}



#endif
