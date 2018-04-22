#ifndef NEW_GUI_BUTTON
#define NEW_GUI_BUTTON

#include "graphics/userinterface/element.hpp"
#include <string>
#include <memory>

namespace BlueBear::Graphics::Vector { class Renderer; }
namespace BlueBear::Graphics::UserInterface::Widgets {

  class Button : public Element {
    std::string label;
    double textSpan = 0.0;

  protected:
    Button( const std::string& id, const std::vector< std::string >& classes, const std::string& innerText );

  public:
    virtual void render( Graphics::Vector::Renderer& renderer ) override;
    virtual void calculate() override;
    virtual void reflow() override;

    static std::shared_ptr< Button > create( const std::string& id, const std::vector< std::string >& classes, const std::string& innerText );
  };

}

#endif
