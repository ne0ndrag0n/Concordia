#ifndef NEW_GUI_SPACER
#define NEW_GUI_SPACER

#include "graphics/userinterface/element.hpp"
#include <vector>
#include <string>
#include <memory>

namespace BlueBear::Graphics::Vector { class Renderer; }
namespace BlueBear::Graphics::UserInterface::Widgets {

  class Spacer : public Element {
  protected:
    Spacer( const std::string& id, const std::vector< std::string >& classes );

  public:
    virtual bool drawableDirty() override;
    virtual void calculate() override;
    virtual void reflow() override;

    static std::shared_ptr< Spacer > create( const std::string& id, const std::vector< std::string >& classes );
  };

}

#endif
