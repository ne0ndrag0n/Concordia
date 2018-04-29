#ifndef NEW_GUI_IMAGE
#define NEW_GUI_IMAGE

#include "graphics/userinterface/element.hpp"
#include "graphics/vector/renderer.hpp"
#include <vector>
#include <string>
#include <memory>
#include <optional>

namespace BlueBear::Graphics::UserInterface::Widgets {

  class Image : public Element {
    std::optional< Vector::Renderer::Image > image;

  protected:
    Image( const std::string& id, const std::vector< std::string >& classes, const std::string& filePath );

  public:
    void calculate() override;
    virtual void render( Graphics::Vector::Renderer& renderer ) override;
    void setImage( const std::string& path );

    static std::shared_ptr< Image > create( const std::string& id, const std::vector< std::string >& classes, const std::string& filePath );
  };

}

#endif
