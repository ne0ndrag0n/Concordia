#ifndef NEW_GUI_LINEAR_LAYOUT
#define NEW_GUI_LINEAR_LAYOUT

#include "graphics/userinterface/element.hpp"
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <memory>
#include <utility>
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
      namespace Widgets {

        class Layout : public Element {

          struct Relations {
            const std::string orientation;
            const std::string flowProperty;
            const std::string perpProperty;
            int cursor;
            const int rFlowSize;
            const int rPerpSize;
            const int aFlowPos;
            const int aPerpPos;
            const int aFlowSize;
            const int aPerpSize;
            const int flowTotalSpace;
            const int flowTotalWeight;
            const int perpSizeAdjusted;
          };

          std::unordered_map< std::shared_ptr< Element >, glm::uvec2 > cachedRequisitions;

        protected:
          Layout( const std::string& id, const std::vector< std::string >& classes );

          Relations getRelations( Gravity gravity, int padding );
          glm::uvec2 getFinalRequisition( std::shared_ptr< Element > prospect );

        public:
          virtual void reflow( bool selectorsInvalidated = true ) override;
          virtual void positionAndSizeChildren();
          virtual bool drawableDirty();
          virtual void generateDrawable();
          virtual void calculate() override;

          static std::shared_ptr< Layout > create( const std::string& id, const std::vector< std::string >& classes );
        };

      }
    }
  }
}

#endif
