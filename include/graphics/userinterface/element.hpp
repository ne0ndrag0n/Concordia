#ifndef NEW_GUI_ELEMENT
#define NEW_GUI_ELEMENT

#include "graphics/userinterface/drawable.hpp"
#include "graphics/userinterface/style/style.hpp"
#include "graphics/userinterface/event/eventbundle.hpp"
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
        bool shadow = false;

        Style::Style localStyle;
        glm::uvec2 requisition;
        glm::ivec4 allocation;
        std::unique_ptr< Drawable > drawable;

        Event::EventBundle eventBundle;

        std::weak_ptr< Element > parentWeak;
        std::vector< std::shared_ptr< Element > > children;

        Element( const std::string& tag, const std::string& id, const std::vector< std::string >& classes );
        Element( const Element& other );
        virtual ~Element();

        glm::ivec2 toRelative( const glm::uvec2& location );
        void setShadow( bool status );
        bool valueIsLiteral( int r );
        virtual bool reuseDrawableInstance();
        virtual bool drawableDirty();
        virtual void generateDrawable();

      public:
        static Device::Display::Adapter::Component::GuiComponent* manager;

        void walk( std::function< void( Element& ) > predicate );

        virtual void positionAndSizeChildren();
        virtual void render( Graphics::Vector::Renderer& renderer );
        virtual void calculate() = 0;

        const std::string& getTag() const;
        const std::string& getId() const;
        std::string generateSelectorString() const;
        bool hasClass( const std::string& clss ) const;
        void sortElements();
        std::vector< std::shared_ptr< Element > > getChildren() const;

        std::shared_ptr< Element > getParent();
        virtual void addChild( std::shared_ptr< Element > child, bool doReflow = true );
        virtual void detach( bool doReflow = true );

        Style::Style& getPropertyList();
        Event::EventBundle& getEventBundle();
        glm::uvec2 getRequisition();
        glm::ivec4 getAllocation();
        glm::ivec2 getAbsolutePosition();
        void setAllocation( const glm::uvec4& allocation, bool doReflow = true );

        std::vector< std::shared_ptr< Element > > getLeafNodes();

        virtual void reflow();
        void paint();
        void draw();
      };

    }
  }
}



#endif
