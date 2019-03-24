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
    float scrollX = 0.0f;
    float scrollY = 0.0f;
    bool forceDirty = false;

    bool getXVisible() const;
    bool getYVisible() const;
    float getXRatio() const;
    float getYRatio() const;
    int getXGutter() const;
    int getYGutter() const;
    int getXSpace() const;
    int getYSpace() const;

  protected:
    Scroll( const std::string& id, const std::vector< std::string >& classes );

    glm::vec4 computeScissor( const glm::vec4& parentScissor, const glm::ivec2& absolutePosition ) override;
    void onMouseDown( Device::Input::Metadata event );
    glm::uvec2 getFinalRequisition( std::shared_ptr< Element > prospect ) const;

    void partialReflow();

  public:
    glm::ivec2 getMax() const;
    int getX() const;
    int getY() const;
    void updateX( int x );
    void updateY( int y );

    virtual bool drawableDirty() override;
    virtual void positionAndSizeChildren() override;
    virtual void render( Graphics::Vector::Renderer& renderer ) override;
    virtual void calculate() override;
    virtual void reflow( bool selectorsInvalidated = true ) override;

    static std::shared_ptr< Scroll > create( const std::string& id, const std::vector< std::string >& classes );
  };

}

#endif
