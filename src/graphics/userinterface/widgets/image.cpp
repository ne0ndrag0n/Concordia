#include "graphics/userinterface/widgets/image.hpp"
#include "device/display/adapter/component/guicomponent.hpp"
#include "log.hpp"

namespace BlueBear::Graphics::UserInterface::Widgets {

  Image::Image( const std::string& id, const std::vector< std::string >& classes, const std::string& filePath ) :
    Element::Element( "Image", id, classes ) {
      visible = false;
      setImage( filePath );
    }

  std::shared_ptr< Image > Image::create( const std::string& id, const std::vector< std::string >& classes, const std::string& filePath ) {
    std::shared_ptr< Image > image( new Image( id, classes, filePath ) );

    return image;
  }

  void Image::calculate() {
    if( image ) {
      visible = true;
      requisition = image->getDimensions();
    } else {
      // There's no image that can be drawn here; none will get drawn
      visible = false;
      requisition = { 0, 0 };
    }
  }

  void Image::render( Graphics::Vector::Renderer& renderer ) {
    // Should never even get here if there is no image
    renderer.drawImage( *image, { 0, 0 } );
  }

  void Image::setImage( const std::string& path ) {
    try {
      image.emplace( manager->getVectorRenderer(), path );
    } catch( Vector::Renderer::Image::InvalidImageException e ) {
      Log::getInstance().warn( "Image::Image", "Failed to construct image for path " + path );
    }
  }

}
