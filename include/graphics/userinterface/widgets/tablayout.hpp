#ifndef NEW_GUI_TAB_LAYOUT
#define NEW_GUI_TAB_LAYOUT

#include "graphics/userinterface/element.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <memory>

namespace BlueBear::Graphics::Vector { class Renderer; }
namespace BlueBear::Graphics::UserInterface::Widgets {

  class TabLayout : public Element {
    std::vector< glm::vec4 > textSpans;
    int hoveredIndex = -1;
    int selectedIndex = 0;

  protected:
    TabLayout( const std::string& id, const std::vector< std::string >& classes );

  public:
    void calculate() override;
    virtual void positionAndSizeChildren() override;
    virtual void render( Graphics::Vector::Renderer& renderer ) override;

    static std::shared_ptr< TabLayout > create( const std::string& id, const std::vector< std::string >& classes );
  };

}

#endif
