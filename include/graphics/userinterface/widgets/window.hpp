#ifndef NEW_WIDGET_WINDOW
#define NEW_WIDGET_WINDOW

#include "graphics/userinterface/element.hpp"
#include <memory>
#include <vector>
#include <string>
#include <functional>

namespace BlueBear {
  namespace Device {
    namespace Input {
      class Metadata;
    }
  }

  namespace Graphics {
    namespace Vector {
      class Renderer;
    }
    namespace UserInterface {
      namespace Widgets {

        class Window : public Element {
          friend class WindowDecoration;

        protected:
          Window( const std::string& id, const std::vector< std::string >& classes, const std::string& windowTitle );

          glm::ivec2 getOrigin();
          glm::ivec2 getDimensions();

          void onMouseDown( Device::Input::Metadata event );
          void onMouseUp( Device::Input::Metadata event );
          void onCloseClick( Device::Input::Metadata event );

        public:
          virtual void addChild( std::shared_ptr< Element > child, bool doReflow = true ) override;
          void positionAndSizeChildren() override;
          virtual void render( Graphics::Vector::Renderer& renderer ) override;
          void calculate() override;

          static std::shared_ptr< Window > create( const std::string& id, const std::vector< std::string >& classes, const std::string& windowTitle );
        };

        class WindowDecoration : public Element {
          std::string windowTitle;
          double textSpan = 0;

        protected:
          WindowDecoration( const std::string& windowTitle );

        public:
          void calculate() override;
          virtual void render( Graphics::Vector::Renderer& renderer ) override;

          static std::shared_ptr< WindowDecoration > create( const std::string& windowTitle );
        };

      }
    }
  }
}


#endif
