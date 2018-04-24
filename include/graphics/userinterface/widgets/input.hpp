#ifndef NEW_GUI_INPUT
#define NEW_GUI_INPUT

#include "graphics/userinterface/element.hpp"
#include <string>
#include <vector>
#include <memory>

namespace BlueBear::Graphics::Vector { class Renderer; }
namespace BlueBear::Graphics::UserInterface::Widgets {

  class Input : public Element {
    std::string hintText;
    std::string contents;
    double textSpan = 0.0;

  protected:
    Input( const std::string& id, const std::vector< std::string >& classes, const std::string& hintText, const std::string& contents );

  public:
    virtual void render( Graphics::Vector::Renderer& renderer ) override;
    virtual void calculate() override;
    virtual void reflow() override;

    static std::shared_ptr< Input > create( const std::string& id, const std::vector< std::string >& classes, const std::string& hintText, const std::string& contents );
  };

}

#endif
