#ifndef NEW_GUI_BUTTON
#define NEW_GUI_BUTTON

#include "graphics/userinterface/element.hpp"
#include <string>
#include <memory>

namespace BlueBear::Device::Input { struct Metadata; }
namespace BlueBear::Graphics::Vector { class Renderer; }
namespace BlueBear::Graphics::UserInterface::Widgets {

  class Button : public Element {
    std::string label;
    double textSpan = 0.0;

  protected:
    Button( const std::string& id, const std::vector< std::string >& classes, const std::string& innerText );

    void onMouseIn( Device::Input::Metadata event );
    void onMouseOut( Device::Input::Metadata event );
    void onMouseDown( Device::Input::Metadata event );
    void onMouseUp( Device::Input::Metadata event );

  public:
    virtual void render( Graphics::Vector::Renderer& renderer ) override;
    virtual void calculate() override;
    virtual void reflow( bool selectorsInvalidated = true ) override;

    static std::shared_ptr< Button > create( const std::string& id, const std::vector< std::string >& classes, const std::string& innerText );
  };

}

#endif
