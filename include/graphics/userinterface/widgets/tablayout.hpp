#ifndef NEW_GUI_TAB_LAYOUT
#define NEW_GUI_TAB_LAYOUT

#include "graphics/userinterface/element.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <memory>

namespace BlueBear::Device::Input { struct Metadata; }
namespace BlueBear::Graphics::Vector { class Renderer; }
namespace BlueBear::Graphics::UserInterface::Widgets {

  class TabLayout : public Element {
    std::vector< glm::vec4 > textSpans;
    int selectedIndex = 0;

  protected:
    TabLayout( const std::string& id, const std::vector< std::string >& classes );

    void onMouseUp( Device::Input::Metadata event );
    void onMouseDown( Device::Input::Metadata event );

  public:
    void calculate() override;
    virtual void positionAndSizeChildren() override;
    virtual void render( Graphics::Vector::Renderer& renderer ) override;

    void selectElement( unsigned int index );

    static std::shared_ptr< TabLayout > create( const std::string& id, const std::vector< std::string >& classes );
  };

}

#endif
