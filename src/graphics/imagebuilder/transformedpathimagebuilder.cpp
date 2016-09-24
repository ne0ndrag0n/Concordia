#include "graphics/imagebuilder/transformedpathimagebuilder.hpp"
#include "graphics/imagebuilder/pathimagebuilder.hpp"
#include <SFML/Graphics.hpp>
#include <string>
#include <memory>
#include <utility>

namespace BlueBear {
  namespace Graphics {

    TransformedPathImageBuilder::TransformedPathImageBuilder( const std::string& path ) : PathImageBuilder::PathImageBuilder( path ) {}

    TransformedPathImageBuilder::TransformedPathImageBuilder( const std::string& path, std::unique_ptr< Transform > transform ) : TransformedPathImageBuilder::TransformedPathImageBuilder( path ) {
      transforms.push_back( std::move( transform ) );
    }

    sf::Image TransformedPathImageBuilder::getImage() {
      sf::Image image = PathImageBuilder::getImage();

      for( auto& transform : transforms ) {
        image = transform->transform( image );
      }

      return image;
    }

    std::string TransformedPathImageBuilder::getKey() {
      if( transforms.size() > 0 ) {
        std::string result = "(";

        unsigned int transformsSize = transforms.size();
        for( unsigned int i = 0; i != transformsSize; i++ ) {
          result = result + transforms[ i ]->getKeyComponent();
          if( i != transformsSize - 1 ) {
            result = result + ":";
          }
        }

        return result + ")";

      } else {
        return PathImageBuilder::getKey();
      }
    }

    void TransformedPathImageBuilder::clearTransforms() {
      transforms.clear();
    }

    void TransformedPathImageBuilder::applyTransform( std::unique_ptr< Transform > transform ) {
      transforms.push_back( std::move( transform ) );
    }

    // Different transforms
    // -----------------------------------------------------------------------------------
    TransformedPathImageBuilder::CropTransform::CropTransform( int x, int y, int w, int h ) : x( x ), y( y ), w( w ), h( h ) {}
    sf::Image TransformedPathImageBuilder::CropTransform::transform( sf::Image& image ) {
      sf::Image result;
      result.create( w, h );

      // Slow copy because we used sf::Image. This entire class should not be used in a tight loop.
      result.copy( image, 0, 0, { x, y, w, h } );

      return result;
    }
    std::string TransformedPathImageBuilder::CropTransform::getKeyComponent() {
      return std::to_string( x ) + "," + std::to_string( y ) + "," + std::to_string( w ) + "," + std::to_string( h );
     }

  }
}
