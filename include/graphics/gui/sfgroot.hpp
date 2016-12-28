#ifndef ROOTWIDGET
#define ROOTWIDGET

#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Container.hpp>
#include <SFML/Graphics.hpp>
#include <string>

namespace BlueBear {
  namespace Graphics {
    namespace GUI {

      class RootContainer : public sfg::Container, sf::NonCopyable {
      public:

          static sfg::Container::Ptr Create();

      private:
          RootContainer() = default;

          virtual const std::string& GetName() const;

          virtual sf::Vector2f CalculateRequisition();
      };

    }
  }
}

#endif
