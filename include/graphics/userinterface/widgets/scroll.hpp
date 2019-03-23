#ifndef NEW_GUI_SCROLL
#define NEW_GUI_SCROLL

#include "graphics/userinterface/element.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <memory>

namespace BlueBear::Graphics::Vector { class Renderer; }
namespace BlueBear::Graphics::UserInterface::Widgets {

  class Scroll : public Element {
    friend class ScrollOverlay;

    float scrollX = 0.0f;
    float scrollY = 0.0f;

    bool addedChildren();
    bool getXScrollSet();
    bool getYScrollSet();
    bool getXVisible();
    bool getYVisible();
    float getXRatio();
    float getYRatio();
    int getXGutter();
    int getYGutter();
    int getXSpace();
    int getYSpace();

    std::shared_ptr< Element > getSingleContainer();

  protected:
    Scroll( const std::string& id, const std::vector< std::string >& classes );

    void updateX( int x );
    void updateY( int y );
    glm::uvec2 getFinalRequisition( std::shared_ptr< Element > prospect );

    void partialReflow();

  public:
    void setChildrenZOrder() override;
    virtual void positionAndSizeChildren() override;
    virtual void calculate() override;
    virtual void reflow( bool selectorsInvalidated = true ) override;

    glm::vec2 getScrollAttributes();

    static std::shared_ptr< Scroll > create( const std::string& id, const std::vector< std::string >& classes );
  };

  // Redesign should have most of the logic encapsulated in ScrollOverlay
  class ScrollOverlay : public Element {
    bool forceDirty = false;

  protected:
    ScrollOverlay();
    void onMouseDown( Device::Input::Metadata event );

  public:
    void regenerate();
    void calculate() override;
    bool drawableDirty() override;
    void reflow( bool selectorsInvalidated = true ) override;
    void render( Graphics::Vector::Renderer& renderer ) override;

    static std::shared_ptr< ScrollOverlay > create();
  };

}

#endif
