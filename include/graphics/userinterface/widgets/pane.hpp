#ifndef NEW_GUI_PANE
#define NEW_GUI_PANE

#include "graphics/userinterface/element.hpp"
#include <vector>
#include <string>
#include <memory>

namespace BlueBear::Graphics::Vector { class Renderer; }
namespace BlueBear::Graphics::UserInterface::Widgets {

  class Pane : public Element {
  protected:
    Pane( const std::string& id, const std::vector< std::string >& classes );

  public:
    virtual void positionAndSizeChildren() override;
    virtual void calculate() override;
    virtual void reflow( bool selectorsInvalidated = true ) override;
    virtual void render( Graphics::Vector::Renderer& renderer ) override;

    static std::shared_ptr< Pane > create( const std::string& id, const std::vector< std::string >& classes );
  };

}

#endif
