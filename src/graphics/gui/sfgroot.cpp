#include "graphics/gui/sfgroot.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace GUI {

      sfg::Container::Ptr RootContainer::Create() {
        return sfg::Container::Ptr( new RootContainer );
      }

      const std::string& RootContainer::GetName() const {
        const static std::string name( "RootContainer" );
        return name;
      }

      sf::Vector2f RootContainer::CalculateRequisition() {
        return sf::Vector2f();
      }

    }
  }
}
